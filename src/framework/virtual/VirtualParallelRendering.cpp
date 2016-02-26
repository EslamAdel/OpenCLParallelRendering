#include "VirtualParallelRendering.h"

#include "Logger.h"

VirtualParallelRendering::VirtualParallelRendering( Volume<uchar> *volume,
                                                    const uint frameWidth,
                                                    const uint frameHeight ,
                                                    const uint virtualGPUsCount)
    : ParallelRendering( volume , frameWidth , frameHeight ) ,
      virtualGPUsCount_( virtualGPUsCount )
{
    LOG_DEBUG("Virtual Parallel Framework Created");
}

void VirtualParallelRendering::addCLRenderer( const uint64_t gpuIndex )
{
    auto *renderer = new VirtualCLRenderer( gpuIndex,
                                            frameWidth_ ,
                                            frameHeight_ ,
                                            transformationAsync_ );

    renderers_.push_back( renderer );
    renderer->setFrameIndex( gpuIndex );

    this->rendererPool_.setMaxThreadCount( renderers_.size( ));
    this->collectorPool_.setMaxThreadCount( renderers_.size( ));

    connect( renderer , SIGNAL( finishedRendering( CLRenderer* )),
             this , SLOT( finishedRendering_SLOT( CLRenderer* )));


}

void VirtualParallelRendering::addCLCompositor(const uint64_t gpuIndex)
{
    LOG_DEBUG("Adding Virtual Compositing Node");


    compositor_ =
            new VirtualCLCompositor( gpuIndex ,
                                     frameWidth_ ,
                                     frameHeight_ );




    this->collagePixmapTask_ =
            new TaskMakePixmap( compositor_->getCLFrameCollage() );

    connect( this->collagePixmapTask_ ,
             SIGNAL( pixmapReady_SIGNAL( QPixmap* , const CLRenderer* )) ,
             this , SLOT(pixmapReady_SLOT( QPixmap* , const CLRenderer* )));
    // Frame index will be assigned to each rendering GPU (rednering node).
    // As a start, consider each frame will be indexed in the next for-loop.


    int frameIndex = 0 ;

    // for each rendering task finished, a collecting task and a
    // compositing task will follow!
    for( VirtualCLRenderer *renderer : renderers_ )
    {

        compositor_->allocateFrame( renderer );

        TaskRender *renderingTask = new TaskRender( *renderer );

        renderingTasks_.push_back( renderingTask );


        // Now add collectingTasks that transfer buffer from The rendering device
        // (rendering node) to the compositing device (compositing node),
        auto collectingTask =
                new VirtualTaskCollect( renderer , compositor_ );

        // Add the collecting task to
        // the map < rendering node , collecting task >
        collectingTasks_.push_back( collectingTask ) ;


        TaskComposite *compositingTask =
                new TaskComposite( compositor_ ,
                                   renderer );

        compositingTasks_.push_back( compositingTask ) ;


        // Map signals from collecting tasks and compositing tasks to the
        // correspondint slots.
        connect( collectingTask ,
                 SIGNAL( frameLoadedToDevice_SIGNAL( VirtualCLRenderer* )) ,
                 this , SLOT( frameLoadedToDevice_SLOT( VirtualCLRenderer* )));

        connect( compositingTask , SIGNAL(compositingFinished_SIGNAL( )) ,
                 this , SLOT(compositingFinished_SLOT( )));

        frameIndex++ ;
    }


    this->compositorSpecified_ = true ;
}

void VirtualParallelRendering::distributeBaseVolume1D()
{
    LOG_DEBUG("Distributing Volume");

    // Handle some minor exceptions.
    if( this->baseVolume_ == nullptr )
        LOG_ERROR( "No Base Volume to distribute!" );

    const int nDevices = renderers_.size();

    if( nDevices == 0 )
        LOG_ERROR( "No deployed devices to distribute volume!");


    //    QVector< Volume8 *> bricks = this->baseVolume_->getBricksXAxis( nDevices );
    QVector< Volume8 *> bricks = baseVolume_->heuristicBricking( nDevices );

    int i = 0;

    for( auto renderer  : renderers_ )
    {
        LOG_DEBUG( "Loading subVolume to device" );

        auto subVolume = bricks[ i++ ];
        renderer->loadVolume( subVolume );

        LOG_DEBUG( "[DONE] Loading subVolume to GPU <%d>",
                   renderer->getGPUIndex( ));
    }
}

int VirtualParallelRendering::getCLRenderersCount() const
{
    return renderers_.size();
}

uint VirtualParallelRendering::getMachineGPUsCount() const
{
    return virtualGPUsCount_;
}

void VirtualParallelRendering::startRendering()
{
    activeRenderers_ =  renderers_.size();

    LOG_INFO("Triggering Rendering Nodes");

    //First spark of the rendering loop.
    applyTransformation_();

    LOG_INFO("[DONE] Triggering Rendering Nodes");
}

void VirtualParallelRendering::frameLoadedToDevice_SLOT( VirtualCLRenderer *renderer )
{
    LOG_DEBUG("Frame<%d> Loaded to Device" , renderer->getGPUIndex() );

    emit this->frameReady_SIGNAL( &renderer->getCLFrame()->getFramePixmap() , renderer  );
    //accumulate the recently loaded frame to the collage frame.
    compositorPool_.start( compositingTasks_[ renderer->getGPUIndex() ] );
}

void VirtualParallelRendering::finishedRendering_SLOT(CLRenderer *finishedRenderer)
{

    collectorPool_.start( collectingTasks_[ finishedRenderer->getGPUIndex() ]);

}

void VirtualParallelRendering::compositingFinished_SLOT()
{
    LOG_DEBUG("[DONE] Compositing");


    //this->pixmapMakerPool_.start( collagePixmapTask_ );

    emit this->finalFrameReady_SIGNAL(
                &compositor_->getCLFrameCollage()->getFramePixmap() );

    if( this->pendingTransformations_ )
        applyTransformation_();
    else
        this->renderersReady_ = true ;
}



void VirtualParallelRendering::applyTransformation_()
{
    this->readyPixmapsCount_ = 0 ;

    // fetch new transformations if exists.
    this->syncTransformation_();

    for( VirtualCLRenderer *renderer : renderers_ )
    {
        LOG_DEBUG("Triggering renderer[%d]:%p" , renderer->getGPUIndex() ,
                  renderer );
        // Spawn threads and start rendering on each rendering node.
        this->rendererPool_.start( renderingTasks_[ renderer->getGPUIndex() ]);
    }

    this->pendingTransformations_ = false;
    this->renderersReady_ = false;
}
