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
                                           volumeDensityAsync_,
                                           brightnessAsync_  );

    renderingNodes_.push_back( node );


    this->rendererPool_.setMaxThreadCount( renderingNodes_.size( ));
    this->collectorPool_.setMaxThreadCount( renderingNodes_.size( ));

    connect( node , SIGNAL( finishedRendering( RenderingNode* )),
             this , SLOT( finishedRendering_SLOT( RenderingNode* )));


}

void VirtualParallelRendering::addCompositingNode(const uint64_t gpuIndex)
{
    LOG_DEBUG("Adding Virtual Compositing Node");
    // add compositingNode_ that will manage compositing rendered frames.
    std::vector< const Coordinates3D *> framesCenters ;
    for( auto node : renderingNodes_ )
        framesCenters.push_back( &node->getCurrentCenter() );


    compositingNode_ =
            new VirtualCompositingNode( gpuIndex ,
                                        renderingNodes_.size() ,
                                        frameWidth_ ,
                                        frameHeight_ ,
                                        framesCenters );




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

        node->setFrameIndex( frameIndex );

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
                                   frameIndex );

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

    // Decompose the base volume for each rendering device
    // evenly over the X-axis.
    const uint64_t baseXDimension = this->baseVolume_->getDimensions().x;
    const uint64_t newXDimension =  baseXDimension / nDevices  ;


    //Extract Brick For each node
    for( auto i = 0 ; i < nDevices - 1 ; i++ )
    {
        auto *brick = this->baseVolume_->getBrick( newXDimension*i ,
                                                   newXDimension*( i + 1 )  ,
                                                   0,
                                                   baseVolume_->getDimensions().y ,
                                                   0,
                                                   baseVolume_->getDimensions().z );

        this->bricks_.push_back( brick );
    }

    //The Last node will have the entire remaining brick
    auto brick = this->baseVolume_->getBrick( newXDimension*( nDevices - 1 ) ,
                                              this->baseVolume_->getDimensions().x ,
                                              0,
                                              this->baseVolume_->getDimensions().y ,
                                              0,
                                              this->baseVolume_->getDimensions().z );

    this->bricks_.push_back( brick );
    int i = 0;

    for( auto node  : renderingNodes_ )
    {
        LOG_DEBUG( "Loading subVolume to device" );

        auto subVolume = this->bricks_[ i++ ];
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
    LOG_DEBUG("Frame Loaded to Device");

    emit this->frameReady_SIGNAL( &node->getCLFrame()->getFramePixmap() , node  );
    //accumulate the recently loaded frame to the collage frame.
    compositorPool_.start( compositingTasks_[ node->getFrameIndex() ] );
}

void VirtualParallelRendering::finishedRendering_SLOT(RenderingNode *node)
{

    collectorPool_.start( collectingTasks_[ node->getFrameIndex() ]);

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
        LOG_DEBUG("Triggering rendering frame[%d]:%p" , node->getFrameIndex() ,
                  node );
        // Spawn threads and start rendering on each rendering node.
        this->rendererPool_.start( renderingTasks_[ node->getFrameIndex() ]);
    }

    this->pendingTransformations_ = false;
    this->renderingNodesReady_ = false;
}