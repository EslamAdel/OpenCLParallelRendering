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
    baseVolume_.reset( volume );

    Dimensions2D frameDimension( frameWidth , frameHeight );

    clFrame_.reset( new CLData::CLImage2D< F >( frameDimension ,
                                                channelOrder ));

    clFrameReadout_.reset( new CLData::CLImage2D< F >( frameDimension ,
                                                       channelOrder ));

    connect( this , SIGNAL(finishedCompositing_SIGNAL()) ,
             this , SLOT(compositingFinished_SLOT()));

    connect( this , SIGNAL(frameReady_SIGNAL( QPixmap*,
                                              const Renderer::CLAbstractRenderer* )) ,
             this, SLOT( pixmapReady_SLOT( QPixmap*,
                                           const Renderer::CLAbstractRenderer*)) ,
             Qt::BlockingQueuedConnection );

}

template< class V , class F >
void SortFirstRenderer< V , F >::addCLRenderer( const uint64_t gpuIndex )
{
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
             SLOT( finishedRendering_SLOT( Renderer::CLAbstractRenderer* )),
             Qt::BlockingQueuedConnection );
}

template< class V , class F >
void SortFirstRenderer< V , F >::addCLCompositor( const uint64_t gpuIndex )
{
    LOG_ERROR("Don't invoke me.");
}

template< class V , class F >
void SortFirstRenderer< V , F >::distributeBaseVolume( )
{
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

    if( renderers_.isEmpty())
    {
        LOG_WARNING("No renderers selected.");
        return;
    }

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

}

template< class V , class F >
void SortFirstRenderer< V , F >::finishedRendering_SLOT(
        Renderer::CLAbstractRenderer *renderer )
{

    QtConcurrent::run(  this , &SortFirstRenderer::assemble_ ,
                        renderer , clFrame_.data( ));


}

template< class V , class F >
void SortFirstRenderer< V , F >::compositingFinished_SLOT( )
{

    if( ! ++assembledFramesCount_ == renderers_.size( ))
        return;

    if( pendingTransformations_ )
        applyTransformation_();
    else
        renderersReady_ = true ;


    //    clone_();
    QtConcurrent::run( this ,  &SortFirstRenderer::clone_ );
}

template< class V , class F >
void SortFirstRenderer< V , F >::frameLoadedToDevice_SLOT(
        Renderer::CLAbstractRenderer *renderer )
{
    LOG_ERROR("Don't Invoke me.");
}

template< class V , class F >
void SortFirstRenderer< V , F >::pixmapReady_SLOT(
        QPixmap *pixmap, const Renderer::CLAbstractRenderer *renderer )
{



    emit this->finalFrameReady_SIGNAL( pixmap );
}


template< class V , class F >
void SortFirstRenderer< V , F >::applyTransformation_()
{

    // fetch new transformations if exists.
    syncTransformation_( );

    for( Renderer::CLAbstractRenderer *renderer : renderers_ )
        QtConcurrent::run( &SortFirstRenderer::render_ ,
                           renderer );


    pendingTransformations_ = false;
    renderersReady_ = false;
    renderedFramesCount_ = 0 ;
    assembledFramesCount_ = 0 ;
}

template< class V , class F >
void SortFirstRenderer< V , F >::benchmark_()
{

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

    //    LOG_DEBUG("Frame[%d]:%s",renderer->getGPUIndex() ,
    //              clFrame->getFrameDimensions().toString().c_str());

    const Dimensions2D finalFrameSize = finalFrame->getFrameDimensions();

    const Dimensions2D &offset = renderer->getSortFirstOffset();
    const Dimensions2D &frameSize = renderer->getSortFirstDimensions();


    F *finalFrameBuffer = finalFrame->getHostData();
    F *frameBuffer = clFrame->getHostData();


    //    LOG_DEBUG("GPU<%d>:offset:%s" ,
    //              renderer->getGPUIndex() ,
    //              offset.toString().c_str());

    //    LOG_DEBUG("GPU<%d>:size:%s",
    //              renderer->getGPUIndex(),
    //              frameSize.toString().c_str());

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

    emit this->compositingFinished_SLOT();
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
