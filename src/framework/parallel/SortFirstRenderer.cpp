#include "SortFirstRenderer.h"
#include "Logger.h"

namespace clparen
{
namespace Parallel
{

template< class V , class F >
SortFirstRenderer< V , F >::SortFirstRenderer( Volume< V > *volume,
                                               const uint64_t frameWidth,
                                               const uint64_t frameHeight )
    : CLAbstractParallelRenderer( frameWidth , frameHeight )
{
    baseVolume_.reset( volume );

    Dimensions2D frameDimension( frameWidth , frameHeight );

    frameBuffer_.reset( new F[ frameDimension.imageSize() ]);
    frame_.reset( new Image< F >( frameDimension , frameBuffer_.data( )));

    clFrame_.reset( new clData::CLImage2D< F >( frameDimension ));

    connect( this , SIGNAL(finishedCompositing_SIGNAL()) ,
             this , SLOT(compositingFinished_SLOT()));

    connect( this ,
             SIGNAL(finalFrameReady_SIGNAL(QPixmap*)) ,
             this,
             SLOT( finalFrameReady_SLOT(QPixmap*)),
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
                        renderer , frame_.data( ) ,
                        frameBuffer_.data( ));


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
void SortFirstRenderer< V , F >::finalFrameReady_SLOT(QPixmap *pixmap)
{
    for( const Renderer::CLAbstractRenderer *it : renderers_ )
    {
        clData::CLImage2D< float > *frame = it->getCLFrame()
                .value< clData::CLImage2D< float >*>();

        emit this->frameReady_SIGNAL( &frame->getFramePixmap() ,
                                      it);
    }

    emit this->frameReady_SIGNAL( pixmap , nullptr );

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
        Image< F > *finalFrame ,
        F *finalFrameBuffer )
{
    clData::CLImage2D< F > *clFrame =
            renderer->getCLFrame().value< clData::CLImage2D< F > *>();

    clFrame->readDeviceData( renderer->getCommandQueue() ,
                             CL_TRUE );

    const Dimensions2D finalFrameSize = finalFrame->getDimensions();

    const Dimensions2D &offset = renderer->getSortFirstOffset();
    const Dimensions2D &frameSize = renderer->getSortFirstDimensions();

    F *frameBuffer = clFrame->getHostData();


    frameCopyMutex_.lockForRead();
    // Frames Assembly.
    for( uint64_t i = 0 ; i < frameSize.x ; i++ )
        for( uint64_t j = 0 ; j < frameSize.y ; j++ )
        {
            const uint64_t x = i + offset.x ;
            const uint64_t y = j + offset.y ;

            // small frame flat index.
            const uint64_t frameIndex =  i +  frameSize.x * j ;
            // big frame flat index.
            const uint64_t finalFrameIndex = x + finalFrameSize.x * y ;

            finalFrameBuffer[ finalFrameIndex ] = frameBuffer[ frameIndex ];
        }

    frameCopyMutex_.unlock();

    emit this->compositingFinished_SLOT();
}


template< class V , class F >
void SortFirstRenderer< V , F >::clone_( )
{

    const F *source = frame_.data()->getData();

    frameCopyMutex_.lockForWrite();
    clFrame_->copyHostData( source );

    emit this->finalFrameReady_SIGNAL( &clFrame_->getFramePixmap( ));
    frameCopyMutex_.unlock();

}





}
}


#include "SortFirstRenderer.ipp"
