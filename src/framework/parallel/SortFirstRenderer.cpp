#include "SortFirstRenderer.h"
#include "Logger.h"


namespace clparen
{
namespace Parallel
{

template< class V , class F >
SortFirstRenderer< V , F >::SortFirstRenderer(
        Volume< V > *volume,
        const uint64_t frameWidth,
        const uint64_t frameHeight ,
        const CLData::FRAME_CHANNEL_ORDER channelOrder )
    : CLAbstractParallelRenderer( frameWidth , frameHeight , channelOrder ),
      renderingLoopCounter_( 0 )
{
    assertThread_();

    baseVolume_.reset( volume );

    Dimensions2D frameDimension( frameWidth , frameHeight );

    clFrame_.reset( new CLData::CLImage2D< F >( frameDimension ,
                                                channelOrder ));

    clFrameReadout_.reset( new CLData::CLImage2D< F >( frameDimension ,
                                                       channelOrder ));


    connect( this , SIGNAL(frameReady_SIGNAL( QPixmap*, uint )) ,
             this, SLOT( pixmapReady_SLOT( QPixmap*, uint )));
}

template< class V , class F >
void SortFirstRenderer< V , F >::addCLRenderer( const uint64_t gpuIndex )
{
    assertThread_();


    // if device already occupied by a rendering node, return.
    if( clRenderers_.keys().contains( gpuIndex )) return;

    if( gpuIndex >= listGPUs_.size() )
    {
        LOG_WARNING("Choose GPU index from [0-%d]", listGPUs_.size() - 1 );
        return;
    }


    Renderer::CLRenderer< V , F > *renderer =
            new Renderer::CLRenderer< V , F >(
                gpuIndex, transformationAsync_ ,
                Dimensions2D( frameWidth_ , frameHeight_ ));


    // Add the rendering node to the map< gpu , rendering node >.
    clRenderers_[ gpuIndex ] = renderer ;
    renderers_[ gpuIndex  ] = renderer;

    rendererPool_.setMaxThreadCount( renderers_.size( ));


    ATTACH_RENDERING_PROFILE( renderer );
    transferTimeMean_[ gpuIndex ] = 0.0 ;
}

template< class V , class F >
void SortFirstRenderer< V , F >::addCLCompositor( const uint64_t gpuIndex )
{
    LOG_ERROR("Don't invoke me.");
}

template< class V , class F >
void SortFirstRenderer< V , F >::distributeBaseVolume( )
{
    assertThread_();


    QVector< QFuture< void >> loadVolumeFuture;

    for( Renderer::CLRenderer< V , F > *renderer : clRenderers_ )
        loadVolumeFuture.append(
                    QtConcurrent::run( renderer ,
                                       &Renderer::CLRenderer< V , F >::loadVolume,
                                       baseVolume_.data( )));

    for( QFuture< void > &future : loadVolumeFuture )
        future.waitForFinished();

}

template< class V , class F >
void SortFirstRenderer< V , F >::initializeRenderers()
{

    assertThread_();


    if( renderers_.isEmpty())
    {
        LOG_WARNING("No renderers selected.");
        return;
    }

    LOG_DEBUG("Sort first frames setting..");
    // Each renderer to contribute with a frame of size:
    // ( Original Width ) X ( Original Height / N )

    // Compensate the round-off error for the last renderer.
    uint64_t sortFirstHeight = frameHeight_ / renderers_.size();

    LOG_DEBUG("Frame size:(%d,%d) [SF:%d]", frameWidth_ , frameHeight_ , sortFirstHeight );

    uint64_t i = 0 ;

    QMap< uint , Renderer::CLAbstractRenderer* >::iterator it = renderers_.begin();

    while( it != renderers_.end( ))
    {
        it.value()->setSortFirstSettings(
                    /* offset */
                    Dimensions2D( 0 , sortFirstHeight * i ),
                    /* region of interest dimensions */
                    Dimensions2D( frameWidth_  ,sortFirstHeight  ));
        it++;
        i++;
    }
    it--; // Now it points to the last renderer.
    i-- ;
    it.value()->setSortFirstSettings(
                /* offset */
                Dimensions2D( 0 , sortFirstHeight * i) ,
                /* region of interest dimensions */
                Dimensions2D( frameWidth_ , frameHeight_ - sortFirstHeight * i ));

    LOG_DEBUG("[DONE] Sort first frames setting..");
}

template< class V , class F >
void SortFirstRenderer< V , F >::finishedRendering_SLOT(
        uint gpuIndex )
{

    assertThread_();


    QtConcurrent::run(  this , &SortFirstRenderer< V , F >::assemble_ ,
                        clRenderers_[ gpuIndex ] , clFrame_.data( ));

}

template< class V , class F >
void SortFirstRenderer< V , F >::compositingFinished_SLOT( )
{

    assertThread_();


    if( ++assembledFramesCount_ != renderers_.size( ))
        return;


    TOC( frameworkProfile.renderingLoop_TIMER );

    calculateTransferTimeMean_();
    renderingLoopCounter_++;


    if( pendingTransformations_ )
        applyTransformation_();
    else
    {
        renderersReady_ = true ;

#ifdef BENCHMARKING
        static uint framesCounter = 0 ;
        if( ++framesCounter < testFrames )
        {
            updateRotationX_SLOT( transformation_.rotation.x + 1 );
            updateRotationY_SLOT( transformation_.rotation.y + 1 );
            updateRotationZ_SLOT( transformation_.rotation.z + 1 );
            //            printf("%d/%d ",framesCounter,testFrames);
        }
        else
            benchmark_();

#endif
    }


#ifndef BENCHMARKING
    TIC( frameworkProfile.convertToPixmap_TIMER );
    Q_EMIT this->frameReady_SIGNAL( &clFrame_->getFramePixmap( ) , 0 );
    TOC( frameworkProfile.convertToPixmap_TIMER );
#endif

}

template< class V , class F >
void SortFirstRenderer< V , F >::frameLoadedToDevice_SLOT( uint )
{
    LOG_ERROR("Don't Invoke me.");
}

template< class V , class F >
void SortFirstRenderer< V , F >::pixmapReady_SLOT( QPixmap *pixmap, uint )
{
    assertThread_();

    Q_EMIT this->finalFrameReady_SIGNAL( pixmap );
}

template< class V , class F >
void SortFirstRenderer< V , F >::setLoadBalancing( bool useLoadBalancing )
{
    useLoadBalancing_ = useLoadBalancing;
}


template< class V , class F >
void SortFirstRenderer< V , F >::applyTransformation_()
{
    assertThread_();


    renderersReady_ = false;
    pendingTransformations_ = false;
    renderedFramesCount_ = 0 ;
    assembledFramesCount_ = 0 ;

    if( useLoadBalancing_ ) heuristicLoadBalance_();

    // fetch new transformations if exists.
    syncTransformation_( );


    TIC( frameworkProfile.renderingLoop_TIMER );

    for( const Renderer::CLRenderer< V , F > *renderer : clRenderers_ )
        QtConcurrent::run( this , &SortFirstRenderer::render_ ,
                           renderer->getGPUIndex( ));

}

template< class V , class F >
void SortFirstRenderer< V , F >::benchmark_()
{
    for( Renderer::CLAbstractRenderer *renderer : renderers_ )
    {
        RENDERING_PROFILE_TAG( renderer );
        PRINT( RENDERING_PROFILE( renderer ).mvMatrix_TIMER );
        PRINT( RENDERING_PROFILE( renderer ).rendering_TIMER );
        printf("Statistics: Transfer Frame from GPU<%lo>: mean=%f\n\n" ,
               renderer->getGPUIndex(),
               transferTimeMean_[ renderer->getGPUIndex() ]);
    }

    //    LOAD_BALANCING_PROFILE_TAG();
    //    PRINT( loadBalancingProfile.loadBlancing_TIMER );

    FRAMEWORK_PROFILE_TAG( );
    PRINT( frameworkProfile.renderingLoop_TIMER );


    EXIT_PROFILING();
}

template< class V , class F >
void SortFirstRenderer< V , F >::heuristicLoadBalance_( )
{
    assertThread_();


    const uint renderersCount = renderers_.size();
    const float tolerance = 0.01f ;
    const float epsilon = 0.001f ;


    if( renderersCount < 2 )
        return;

    QVector< float > renderingTime;
    QVector< float > frameHeightChange;
    uint i = 0 ;
    // 1.Get Rendering Times.
    float meanTime = 0.f ;
    for( const Renderer::CLAbstractRenderer *renderer : renderers_ )
    {
        float time = static_cast< float >( renderer->getRenderingTime( ));
        uint64_t frameHeight =  renderer->getSortFirstDimensions().y ;
        frameHeightChange.append( static_cast< float >( frameHeight ));
        renderingTime.append( time );
        meanTime += time;

        //        printf("time[%d]:%f,size:%dx%d\n",
        //               static_cast< int >(renderer->getGPUIndex()),
        //               time ,
        //               static_cast< int >( frameWidth ),
        //               static_cast< int >( renderer->getFrameDimensions().y ));
    }
    //   printf("\n");


    meanTime /= static_cast< float >( renderersCount );
    float variance = 0.f;
    for( const float time : renderingTime )
        variance += ( time - meanTime ) * ( time - meanTime );
    variance /= renderersCount ;


    // 2.Calculate imbalance factors.
    float positiveGap = 0.f ;  // Underload measure.
    float negativeGap = 0.f ;  // Overload measure.
    for( i = 0 ; i < renderersCount ; i++ )
    {
        float ratio = renderingTime[ i ] / meanTime ;
        if( ratio >  1.f + tolerance )
            ratio = 1.f + tolerance ;
        else if( ratio < 1.f - tolerance )
            ratio = 1.f - tolerance ;

        frameHeightChange[ i ] *= ( 1.f - ratio ) ;

        //        LOG_DEBUG("delta=%f", frameWidthChange[i] );

        if( frameHeightChange[ i ] > 0 )
            positiveGap += frameHeightChange[ i ];
        else
            negativeGap -= frameHeightChange[ i ];
    }


    // 3. If balanced enough (gap < epsilon), return.
    if( positiveGap + negativeGap < 2 * epsilon * frameWidth_ )
        return;


    // 4. Compute the actual changes in overloaded/underloaded frames.
    if( negativeGap > positiveGap)
        for( i = 0 ; i < renderersCount ; i++ )
        {
            if( frameHeightChange[ i ] < 0 )
                frameHeightChange[ i ] =
                        positiveGap * ( frameHeightChange[ i ] / negativeGap );
        }
    else
        for( i = 0 ; i < renderersCount ; i++ )
            if( frameHeightChange[ i ] > 0 )
                frameHeightChange[ i ] =
                        negativeGap * ( frameHeightChange[ i ] / positiveGap );


    // 5.Set new frames widths.
    i = 0 ;
    uint64_t currentOffset = 0 ;

    for( Renderer::CLAbstractRenderer *renderer : renderers_ )
    {
        const uint64_t newFrameHeight =
                renderer->getSortFirstDimensions().y +
                static_cast< int64_t >( frameHeightChange[ i ] );

        if( i < renderersCount - 1 )
            renderer->setSortFirstSettings(
                        /* offset */
                        Dimensions2D( 0 , currentOffset ) ,
                        /* region of interest dimensions */
                        Dimensions2D( frameWidth_ , newFrameHeight ));
        else
            renderer->setSortFirstSettings(
                        /* offset */
                        Dimensions2D( 0 , currentOffset ) ,
                        /* region of interest dimensions */
                        Dimensions2D( frameWidth_  , frameHeight_ - currentOffset ));

        i++;
        currentOffset += newFrameHeight ;
    }
    //    LOG_DEBUG("mean:%f,variance:%f",meanTime, variance);
}

template< class V , class F >
void SortFirstRenderer< V , F >::calculateTransferTimeMean_()
{

    assertThread_();


    if( renderingLoopCounter_ == 0 )
        return;

    for( const Renderer::CLAbstractRenderer *renderer : renderers_ )
    {
        const uint64_t gpuIndex = renderer->getGPUIndex();
        const CLData::CLImage2D< F > *clFrame =
                renderer->getCLFrame().value< CLData::CLImage2D< F > *>();
        const float transferTime = clFrame->getTransferTime();
        //        printf("tranfer time: %f\n",transferTime);
        transferTimeMean_[ gpuIndex ] =
                ( transferTimeMean_[ gpuIndex ] * renderingLoopCounter_ +
                  transferTime ) / ( renderingLoopCounter_ + 1 ) ;
    }

}


template< class V , class F >
void SortFirstRenderer< V , F >::SortFirstRenderer::render_( uint gpuIndex )
{
    clRenderers_[ gpuIndex ]->applyTransformation();

    Q_EMIT finishedRendering_SIGNAL( gpuIndex );
}

template< class V , class F >
void SortFirstRenderer< V , F >::assemble_(
        Renderer::CLRenderer< V , F >  *renderer,
        CLData::CLFrame< F > *finalFrame )
{
    const CLData::CLImage2D< F > &clFrame = renderer->getCLImage2D();

    frameCopyMutex_.lockForRead();
    finalFrame->readOtherDeviceData( renderer->getCommandQueue() ,
                                     clFrame , CL_TRUE );
    frameCopyMutex_.unlock();

    Q_EMIT compositingFinished_SIGNAL();
}


template< class V , class F >
void SortFirstRenderer< V , F >::clone_( )
{
#ifdef BENCHMARKING
    LOG_ERROR("This not to be invoked in benchmarking mode");
#endif

    frameCopyMutex_.lockForWrite();
    clFrameReadout_->copyHostData( *clFrame_.data( ));
    Q_EMIT this->frameReady_SIGNAL( &clFrameReadout_->getFramePixmap( ) , 0 );
    frameCopyMutex_.unlock();
}

}
}


#include "SortFirstRenderer.ipp"
