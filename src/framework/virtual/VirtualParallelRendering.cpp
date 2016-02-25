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

void VirtualParallelRendering::addRenderingNode( const uint64_t gpuIndex )
{
    auto *node = new VirtualRenderingNode( gpuIndex,
                                           frameWidth_ ,
                                           frameHeight_ ,
                                           translationAsync_,
                                           rotationAsync_,
                                           scaleAsync_,
                                           volumeDensityAsync_,
                                           brightnessAsync_  ,
                                           transferFunctionScaleAsync_,
                                           transferFunctionOffsetAsync_ );

    renderingNodes_.push_back( node );
    node->setFrameIndex( gpuIndex );

    this->rendererPool_.setMaxThreadCount( renderingNodes_.size( ));
    this->collectorPool_.setMaxThreadCount( renderingNodes_.size( ));

    connect( node , SIGNAL( finishedRendering( RenderingNode* )),
             this , SLOT( finishedRendering_SLOT( RenderingNode* )));


}

void VirtualParallelRendering::addCompositingNode(const uint64_t gpuIndex)
{
    LOG_DEBUG("Adding Virtual Compositing Node");


    compositingNode_ =
            new VirtualCompositingNode( gpuIndex ,
                                        frameWidth_ ,
                                        frameHeight_ );




    this->collagePixmapTask_ =
            new TaskMakePixmap( compositingNode_->getCLFrameCollage() );

    connect( this->collagePixmapTask_ ,
             SIGNAL( pixmapReady_SIGNAL( QPixmap* , const RenderingNode* )) ,
             this , SLOT(pixmapReady_SLOT( QPixmap* , const RenderingNode* )));
    // Frame index will be assigned to each rendering GPU (rednering node).
    // As a start, consider each frame will be indexed in the next for-loop.


    int frameIndex = 0 ;

    // for each rendering task finished, a collecting task and a
    // compositing task will follow!
    for( VirtualRenderingNode *node : renderingNodes_ )
    {

        compositingNode_->allocateFrame( node );

        TaskRender *renderingTask = new TaskRender( *node );

        renderingTasks_.push_back( renderingTask );


        // Now add collectingTasks that transfer buffer from The rendering device
        // (rendering node) to the compositing device (compositing node),
        auto collectingTask =
                new VirtualTaskCollect( node , compositingNode_ );

        // Add the collecting task to
        // the map < rendering node , collecting task >
        collectingTasks_.push_back( collectingTask ) ;


        TaskComposite *compositingTask =
                new TaskComposite( compositingNode_ ,
                                   node );

        compositingTasks_.push_back( compositingTask ) ;


        // Map signals from collecting tasks and compositing tasks to the
        // correspondint slots.
        connect( collectingTask ,
                 SIGNAL( frameLoadedToDevice_SIGNAL( VirtualRenderingNode* )) ,
                 this , SLOT( frameLoadedToDevice_SLOT( VirtualRenderingNode* )));

        connect( compositingTask , SIGNAL(compositingFinished_SIGNAL( )) ,
                 this , SLOT(compositingFinished_SLOT( )));

        frameIndex++ ;
    }


    this->compositingNodeSpecified_ = true ;
}

void VirtualParallelRendering::distributeBaseVolume1D()
{
    LOG_DEBUG("Distributing Volume");

    // Handle some minor exceptions.
    if( this->baseVolume_ == nullptr )
        LOG_ERROR( "No Base Volume to distribute!" );

    const int nDevices = renderingNodes_.size();

    if( nDevices == 0 )
        LOG_ERROR( "No deployed devices to distribute volume!");


//    QVector< Volume8 *> bricks = this->baseVolume_->getBricksXAxis( nDevices );
    QVector< Volume8 *> bricks = baseVolume_->heuristicBricking( nDevices );

    int i = 0;

    for( auto node  : renderingNodes_ )
    {
        LOG_DEBUG( "Loading subVolume to device" );

        auto subVolume = bricks[ i++ ];
        node->loadVolume( subVolume );

        LOG_DEBUG( "[DONE] Loading subVolume to GPU <%d>",
                   node->getGPUIndex( ));
    }
}

int VirtualParallelRendering::getRenderingNodesCount() const
{
    return renderingNodes_.size();
}

uint VirtualParallelRendering::getMachineGPUsCount() const
{
    return virtualGPUsCount_;
}

void VirtualParallelRendering::startRendering()
{
    activeRenderingNodes_ =  renderingNodes_.size();

    LOG_INFO("Triggering Rendering Nodes");

    //First spark of the rendering loop.
    applyTransformation_();

    LOG_INFO("[DONE] Triggering Rendering Nodes");
}

void VirtualParallelRendering::frameLoadedToDevice_SLOT( VirtualRenderingNode *node )
{
    LOG_DEBUG("Frame<%d> Loaded to Device" , node->getGPUIndex() );

    emit this->frameReady_SIGNAL( &node->getCLFrame()->getFramePixmap() , node  );
    //accumulate the recently loaded frame to the collage frame.
    compositorPool_.start( compositingTasks_[ node->getGPUIndex() ] );
}

void VirtualParallelRendering::finishedRendering_SLOT(RenderingNode *node)
{

    collectorPool_.start( collectingTasks_[ node->getGPUIndex() ]);

}

void VirtualParallelRendering::compositingFinished_SLOT()
{
    LOG_DEBUG("[DONE] Compositing");


    //this->pixmapMakerPool_.start( collagePixmapTask_ );

    emit this->finalFrameReady_SIGNAL(
                &compositingNode_->getCLFrameCollage()->getFramePixmap() );

    if( this->pendingTransformations_ )
        applyTransformation_();
    else
        this->renderingNodesReady_ = true ;
}



void VirtualParallelRendering::applyTransformation_()
{
    this->readyPixmapsCount_ = 0 ;

    // fetch new transformations if exists.
    this->syncTransformation_();

    for( VirtualRenderingNode *node : renderingNodes_ )
    {
        LOG_DEBUG("Triggering renderer[%d]:%p" , node->getGPUIndex() ,
                  node );
        // Spawn threads and start rendering on each rendering node.
        this->rendererPool_.start( renderingTasks_[ node->getGPUIndex() ]);
    }

    this->pendingTransformations_ = false;
    this->renderingNodesReady_ = false;
}
