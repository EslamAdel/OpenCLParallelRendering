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
    : CLAbstractParallelRenderer( frameWidth , frameHeight , channelOrder )
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


    // Map the signal emitted from rendering node after rendering is finished
    // to the corresponding slot.
    connect( renderer ,
             SIGNAL( finishedRendering( Renderer::CLAbstractRenderer* )),
             this ,
             SLOT( finishedRendering_SLOT( Renderer::CLAbstractRenderer* )));


    ATTACH_RENDERING_PROFILE( renderer );
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
    {
        loadVolumeFuture.append(
                    QtConcurrent::run( renderer ,
                                       &Renderer::CLAbstractRenderer::loadVolume,
                                       volume ));
    }


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
    // ( Original Width / N ) X ( Original Height )

    // Compensate the round-off error for the last renderer.
    uint64_t sortFirstWidth = frameWidth_ / renderers_.size();


    uint64_t i = 0 ;
    CLRenderers::iterator it = renderers_.begin();

    while( it != renderers_.end( ))
    {
        it.value()->setSortFirstSettings(
                    /* offset */
                    Dimensions2D( sortFirstWidth * i , 0) ,
                    /* region of interest dimensions */
                    Dimensions2D( sortFirstWidth     , frameHeight_ ));
        it++;
        i++;
    }
    it--; // Now it points to the last renderer.
    i-- ;
    it.value()->setSortFirstSettings(
                /* offset */
                Dimensions2D( sortFirstWidth * i , 0) ,
                /* region of interest dimensions */
                Dimensions2D( frameWidth_ - sortFirstWidth * i , frameHeight_ ));

    LOG_DEBUG("[DONE] Sort first frames setting..");
}

template< class V , class F >
void SortFirstRenderer< V , F >::finishedRendering_SLOT(
        Renderer::CLAbstractRenderer *renderer )
{

    if( QThread::currentThread() != this->thread())
        LOG_ERROR("Foreign thread!");


//#ifdef BENCHMARKING
//    static int renderedFrames = 0 ;
//    if( ++rendererdFrames == 1 )
//        TIC( compositingProfile.compositing_TIMER );
//    else if( renderedFrames == renderers_.size())
//        renderedFrames = 0 ;

//#endif


    QtConcurrent::run(  this , &SortFirstRenderer::assemble_ ,
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
        }
        else
            benchmark_();

#endif
    }

#ifndef BENCHMARKING
    QtConcurrent::run( this ,  &SortFirstRenderer::clone_ );
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
void SortFirstRenderer< V , F >::applyTransformation_()
{
    if( QThread::currentThread() != this->thread())
        LOG_ERROR("Foreign thread!");

    TIC( frameworkProfile.renderingLoop_TIMER );

    renderersReady_ = false;
    pendingTransformations_ = false;
    renderedFramesCount_ = 0 ;
    assembledFramesCount_ = 0 ;


    TIC( loadBalancingProfile.loadBlancing_TIMER );
    heuristicLoadBalance_();
    TOC( loadBalancingProfile.loadBlancing_TIMER );

    // fetch new transformations if exists.
    syncTransformation_( );


    for( Renderer::CLAbstractRenderer *renderer : renderers_ )
        QtConcurrent::run( &SortFirstRenderer::render_ ,
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
    }

    LOAD_BALANCING_PROFILE_TAG();
    PRINT( loadBalancingProfile.loadBlancing_TIMER );

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

    const float tolerance = 0.5f ;
    const float epsilon = 0.05 ;

    if( renderersCount < 2 )
        return;

    QVector< float > renderingTime;
    QVector< float > frameWidthChange;
    uint i = 0 ;
    // 1.Get Rendering Times.
    float meanTime = 0.f ;
    for( const Renderer::CLAbstractRenderer *renderer : renderers_ )
    {
//        LOG_DEBUG("%d:%p",i++, QThread::currentThread());
        float time = static_cast< float >( renderer->getRenderingTime( ));
        uint64_t frameWidth =  renderer->getSortFirstDimensions().x ;
//        LOG_DEBUG("Last Width:%ld, time:%f", frameWidth , time );
        frameWidthChange.append( static_cast< float >( frameWidth ));
        renderingTime.append( time );
        meanTime += time;

        printf("time[%d]:%f,size:%dx%d\n",
               static_cast< int >(renderer->getGPUIndex()),
               time ,
               static_cast< int >( frameWidth ),
               static_cast< int >( renderer->getFrameDimensions().y ));
    }

    printf("\n");


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

        frameWidthChange[ i ] *= ( 1.f - ratio );

//        LOG_DEBUG("delta=%f", frameWidthChange[i] );

        if( frameWidthChange[ i ] > 0 )
            positiveGap += frameWidthChange[ i ];
        else
            negativeGap -= frameWidthChange[ i ];
    }


    // 3. If balanced enough (gap < epsilon), return.
    if( positiveGap + negativeGap < 2 * epsilon * frameWidth_ )
    {
//        LOG_DEBUG("Gap = %f < %f ", positiveGap + negativeGap ,2 * epsilon * frameWidth_ );
        return;
    }

    // 4. Compute the actual changes in overloaded/underloaded frames.
    if( negativeGap > positiveGap)
        for( i = 0 ; i < renderersCount ; i++ )
        {
            if( frameWidthChange[ i ] < 0 )
                frameWidthChange[ i ] =
                        positiveGap * ( frameWidthChange[ i ] / negativeGap );
        }
    else
        for( i = 0 ; i < renderersCount ; i++ )
            if( frameWidthChange[ i ] > 0 )
                frameWidthChange[ i ] =
                        negativeGap * ( frameWidthChange[ i ] / positiveGap );


    // 5.Set new frames widths.
    i = 0 ;
    uint64_t currentOffset = 0 ;

    for( Renderer::CLAbstractRenderer *renderer : renderers_ )
    {
//        LOG_DEBUG("New framewidth: %ld + %f = %ld, offset = %ld",
//                  renderer->getSortFirstDimensions().x ,
//                  frameWidthChange[ i ] ,
//                  renderer->getSortFirstDimensions().x +
//                  static_cast< int64_t >( frameWidthChange[ i ] ) ,
//                  currentOffset );

        const uint64_t newFrameWidth =
                renderer->getSortFirstDimensions().x +
                static_cast< int64_t >( frameWidthChange[ i ] );

        if( i < renderersCount - 1 )
            renderer->setSortFirstSettings(
                        /* offset */
                        Dimensions2D( currentOffset , 0) ,
                        /* region of interest dimensions */
                        Dimensions2D( newFrameWidth , frameHeight_ ));
        else
            renderer->setSortFirstSettings(
                        /* offset */
                        Dimensions2D( currentOffset , 0) ,
                        /* region of interest dimensions */
                        Dimensions2D( frameWidth_ - currentOffset , frameHeight_ ));

        i++;
        currentOffset += newFrameWidth ;
    }

//    LOG_DEBUG("Gap:%f, thread:%p", positiveGap + negativeGap , QThread::currentThread( ));

    LOG_DEBUG("mean:%f,variance:%f",meanTime, variance);
}


template< class V , class F >
void SortFirstRenderer< V , F >::SortFirstRenderer::render_(
        Renderer::CLAbstractRenderer *renderer )
{
    renderer->applyTransformation();
}

template< class V , class F >
void SortFirstRenderer< V , F >::assemble_(
        Renderer::CLAbstractRenderer *renderer ,
        CLData::CLFrame< F > *finalFrame )
{
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
void SortFirstRenderer< V , F >::clone_( )
{
    frameCopyMutex_.lockForWrite();
    clFrameReadout_->copyHostData( *clFrame_.data( ));
    emit this->frameReady_SIGNAL( &clFrame_->getFramePixmap( ) , nullptr );
    frameCopyMutex_.unlock();
}





}
}


#include "SortFirstRenderer.ipp"
