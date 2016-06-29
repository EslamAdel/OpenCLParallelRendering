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
    if( QThread::currentThread() != this->thread())
        LOG_ERROR("Foreign thread!");

    baseVolume_.reset( volume );

    Dimensions2D frameDimension( frameWidth , frameHeight );

    clFrame_.reset( new CLData::CLImage2D< F >( frameDimension ,
                                                channelOrder ));

    clFrameReadout_.reset( new CLData::CLImage2D< F >( frameDimension ,
                                                       channelOrder ));

    connect( this , SIGNAL(finishedCompositing_SIGNAL( )) ,
             this , SLOT(compositingFinished_SLOT( )));

    connect( this , SIGNAL(frameReady_SIGNAL( QPixmap*,
                                              const Renderer::CLAbstractRenderer* )) ,
             this, SLOT( pixmapReady_SLOT( QPixmap*,
                                           const Renderer::CLAbstractRenderer*)));

}

template< class V , class F >
void SortFirstRenderer< V , F >::addCLRenderer( const uint64_t gpuIndex )
{
    if( QThread::currentThread() != this->thread())
        LOG_ERROR("Foreign thread!");

    // if device already occupied by a rendering node, return.
    if( renderers_.keys().contains( listGPUs_.at( gpuIndex ))) return;

    if( gpuIndex >= listGPUs_.size() )
    {
        LOG_WARNING("Choose GPU index from [0-%d]", listGPUs_.size() - 1 );
        return;
    }

    inUseGPUs_ << listGPUs_.at( gpuIndex );

    Renderer::CLAbstractRenderer *renderer =
            new Renderer::CLRenderer< V , F >(
                gpuIndex, transformationAsync_ ,
                Dimensions2D( frameWidth_ , frameHeight_ ));


    // Add the rendering node to the map< gpu , rendering node >.
    renderers_[ listGPUs_.at( gpuIndex ) ] = renderer;

    rendererPool_.setMaxThreadCount( renderers_.size( ));

    connect( this ,
             SIGNAL( finishedRendering_SIGNAL( Renderer::CLAbstractRenderer* )),
             this ,
             SLOT( finishedRendering_SLOT( Renderer::CLAbstractRenderer* )));


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
    if( QThread::currentThread() != this->thread())
        LOG_ERROR("Foreign thread!");

    VolumeVariant volume = VolumeVariant::fromValue( baseVolume_.data( ));


    QVector< QFuture< void >> loadVolumeFuture;

    for( Renderer::CLAbstractRenderer *renderer : renderers_ )
        loadVolumeFuture.append(
                    QtConcurrent::run( renderer ,
                                       &Renderer::CLAbstractRenderer::loadVolume,
                                       volume ));

    for( QFuture< void > &future : loadVolumeFuture )
        future.waitForFinished();

}

template< class V , class F >
void SortFirstRenderer< V , F >::initializeRenderers()
{

    if( QThread::currentThread() != this->thread())
        LOG_ERROR("Foreign thread!");

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
    CLRenderers::iterator it = renderers_.begin();

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
        Renderer::CLAbstractRenderer *renderer )
{

    if( QThread::currentThread() != this->thread())
        LOG_ERROR("Foreign thread!");



    QtConcurrent::run(  this , &SortFirstRenderer::assemble2_ ,
                        renderer , clFrame_.data( ));

}

template< class V , class F >
void SortFirstRenderer< V , F >::compositingFinished_SLOT( )
{

    if( QThread::currentThread() != this->thread())
        LOG_ERROR("Foreign thread!");

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
    emit this->frameReady_SIGNAL( &clFrame_->getFramePixmap( ) , nullptr );
    TOC( frameworkProfile.convertToPixmap_TIMER );
//    QtConcurrent::run( this ,  &SortFirstRenderer::clone_ );
#endif

}

template< class V , class F >
void SortFirstRenderer< V , F >::frameLoadedToDevice_SLOT(
        Renderer::CLAbstractRenderer * )
{
    LOG_ERROR("Don't Invoke me.");
}

template< class V , class F >
void SortFirstRenderer< V , F >::pixmapReady_SLOT(
        QPixmap *pixmap, const Renderer::CLAbstractRenderer *renderer )
{
    if( QThread::currentThread() != this->thread())
        LOG_ERROR("Foreign thread!");


    emit this->finalFrameReady_SIGNAL( pixmap );
}

template< class V , class F >
void SortFirstRenderer< V , F >::setLoadBalancing( bool useLoadBalancing )
{
    useLoadBalancing_ = useLoadBalancing;
}


template< class V , class F >
void SortFirstRenderer< V , F >::applyTransformation_()
{
    if( QThread::currentThread() != this->thread())
        LOG_ERROR("Foreign thread!");


    renderersReady_ = false;
    pendingTransformations_ = false;
    renderedFramesCount_ = 0 ;
    assembledFramesCount_ = 0 ;

    if( useLoadBalancing_ ) heuristicLoadBalance_();

    // fetch new transformations if exists.
    syncTransformation_( );


    TIC( frameworkProfile.renderingLoop_TIMER );

    for( Renderer::CLAbstractRenderer *renderer : renderers_ )
        QtConcurrent::run( this , &SortFirstRenderer::render_ ,
                           renderer );

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
    if( QThread::currentThread() != this->thread())
        LOG_ERROR("Foreign thread!");

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

    if( QThread::currentThread() != this->thread())
        LOG_ERROR("Foreign thread!");

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
void SortFirstRenderer< V , F >::SortFirstRenderer::render_(
        Renderer::CLAbstractRenderer *renderer )
{
    renderer->applyTransformation();

    emit this->finishedRendering_SIGNAL( renderer );
}

template< class V , class F >
void SortFirstRenderer< V , F >::assemble_(
        Renderer::CLAbstractRenderer *renderer ,
        CLData::CLFrame< F > *finalFrame )
{
#ifdef BENCHMARKING
    LOG_ERROR("This not to be invoked in benchmarking mode");
#endif

    CLData::CLImage2D< F > *clFrame =
            renderer->getCLFrame().value< CLData::CLImage2D< F > *>();

    clFrame->readDeviceData( renderer->getCommandQueue() ,
                             CL_TRUE );

    const Dimensions2D finalFrameSize = finalFrame->getFrameDimensions();

    const Dimensions2D &offset = renderer->getSortFirstOffset();
    const Dimensions2D &frameSize = renderer->getSortFirstDimensions();

    if( offset.x + frameSize.x > frameWidth_  ||
            offset.y + frameSize.y > frameHeight_ )
        LOG_ERROR("Main frame exceeded!");

    F *finalFrameBuffer = finalFrame->getHostData();
    F *frameBuffer = clFrame->getHostData();

    frameCopyMutex_.lockForRead();

    if( clFrame->channelOrder() == CLData::FRAME_CHANNEL_ORDER::ORDER_INTENSITY )
        // Frames Assembly.
        for( uint64_t i = 0 ; i < frameSize.x ; i++ )
        {
            const uint64_t x = i + offset.x ;
            for( uint64_t j = 0 ; j < frameSize.y ; j++ )
            {
                const uint64_t y = j + offset.y ;

                // small frame flat index.
                const uint64_t frameIndex =  i +  frameSize.x * j ;
                // big frame flat index.
                const uint64_t finalFrameIndex = x + finalFrameSize.x * y ;

                finalFrameBuffer[ finalFrameIndex ] = frameBuffer[ frameIndex ];
            }
        }

    else if ( clFrame->channelOrder() ==
              CLData::FRAME_CHANNEL_ORDER::ORDER_RGBA )
        for( uint64_t i = 0 ; i < frameSize.x ; i++ )
        {
            const uint64_t x = i + offset.x ;
            for( uint64_t j = 0 ; j < frameSize.y ; j++ )
            {
                const uint64_t y = j + offset.y ;

                // small frame flat index.
                const uint64_t frameIndex =  i +  frameSize.x * j ;
                // big frame flat index.
                const uint64_t finalFrameIndex = x + finalFrameSize.x * y ;

                finalFrameBuffer[ finalFrameIndex * 4 ] =
                        frameBuffer[ frameIndex * 4 ];

                finalFrameBuffer[ finalFrameIndex * 4 + 1 ] =
                        frameBuffer[ frameIndex * 4 + 1 ];

                finalFrameBuffer[ finalFrameIndex * 4 + 2 ] =
                        frameBuffer[ frameIndex * 4 + 2 ];

                finalFrameBuffer[ finalFrameIndex * 4 + 3 ] =
                        frameBuffer[ frameIndex * 4 + 3 ];
            }
        }
    frameCopyMutex_.unlock();

    emit this->finishedCompositing_SIGNAL();
}

template< class V , class F >
void SortFirstRenderer< V , F >::assemble2_(
        Renderer::CLAbstractRenderer *renderer,
        CLData::CLFrame< F > *finalFrame )
{
    CLData::CLImage2D< F > *clFrame =
            renderer->getCLFrame().value< CLData::CLImage2D< F > *>();

    frameCopyMutex_.lockForRead();
    finalFrame->readOtherDeviceData( renderer->getCommandQueue() ,
                                     *clFrame , CL_TRUE );
    frameCopyMutex_.unlock();

    emit this->finishedCompositing_SIGNAL();
}


template< class V , class F >
void SortFirstRenderer< V , F >::clone_( )
{
#ifdef BENCHMARKING
    LOG_ERROR("This not to be invoked in benchmarking mode");
#endif

    frameCopyMutex_.lockForWrite();
    clFrameReadout_->copyHostData( *clFrame_.data( ));
    emit this->frameReady_SIGNAL( &clFrameReadout_->getFramePixmap( ) , nullptr );
    frameCopyMutex_.unlock();
}

}
}


#include "SortFirstRenderer.ipp"
