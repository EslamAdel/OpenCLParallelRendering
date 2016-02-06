#include "ParallelRendering.h"
#include <Logger.h>
#include "Typedefs.hh"
#include <QList>

#define VOLUME_PREFIX "/projects/volume-datasets/foot/foot"
#define INITIAL_VOLUME_CENTER_X 0.0
#define INITIAL_VOLUME_CENTER_Y 0.0
#define INITIAL_VOLUME_CENTER_Z -4.0
#define INITIAL_VOLUME_ROTATION_X 0.0
#define INITIAL_VOLUME_ROTATION_Y 0.0
#define INITIAL_VOLUME_ROTATION_Z 0.0


//#define BENCH_MARKING

#ifdef BENCH_MARKING
#define AUTO_ROTATE
#define TEST_FRAMES 500
#endif


//Profiles Difinitions
RenderingProfiles renderingProfiles = RenderingProfiles() ;
CollectingProfiles collectingProfiles = CollectingProfiles() ;
CompositingProfile compositingProfile = CompositingProfile() ;
FrameworkProfile frameworkProfile = FrameworkProfile() ;

ParallelRendering::ParallelRendering( Volume<uchar> *volume ,
                                      const uint frameWidth ,
                                      const uint frameHeight )
    : baseVolume_( volume ),
      frameWidth_( frameWidth ),
      frameHeight_( frameHeight ),
      renderingNodesReady_( false ),
      compositingNodeSpecified_( false ),
      compositedFramesCount_( 0 )
{

    listGPUs_ = clHardware_.getListGPUs();
    machineGPUsCount_ = listGPUs_.size();

    // Translation
    translation_.x = INITIAL_VOLUME_CENTER_X;
    translation_.y = INITIAL_VOLUME_CENTER_X;
    translation_.z = INITIAL_VOLUME_CENTER_Z;

    // Rotation
    rotation_.x = INITIAL_VOLUME_ROTATION_X;
    rotation_.y = INITIAL_VOLUME_ROTATION_Y;
    rotation_.z = INITIAL_VOLUME_ROTATION_Z;


    /**
    compositing of frames is not performed concurrently,
    so only one thread will accumulate each frame then upload the
    collage buffer to host, then rewind (clear) the collage frame
    at host for the next round.
      **/
    compositorPool_.setMaxThreadCount( 1 );
}

void ParallelRendering::discoverAllNodes()
{

    for(uint64_t gpuIndex = 0 ;  gpuIndex < listGPUs_.size() ; gpuIndex++ )
        addRenderingNode( gpuIndex );

}

void ParallelRendering::addRenderingNode( const uint64_t gpuIndex)
{
    // if device already occupied by a rendering node, return.
    if( inUseGPUs_.contains( listGPUs_.at( gpuIndex ))) return;


    // pass transformations by reference.
    // translationAsync_, rotationAsync_, volumeDensityAsync_, brightnessAsync_
    // will be accessed by multithreads. So,
    // they should not be modified during the life of the threads.

    RenderingNode *node = new RenderingNode( gpuIndex,
                                             frameWidth_ ,
                                             frameHeight_ ,
                                             translationAsync_,
                                             rotationAsync_,
                                             volumeDensityAsync_,
                                             brightnessAsync_  );

    renderingProfiles[ node ] = new RenderingProfile;
    collectingProfiles[ node ] = new CollectingProfile;

    auto attachedGPU = listGPUs_.at( gpuIndex );

    // Add the recently attached GPU to Set of usedGPUs.
    inUseGPUs_ << attachedGPU;

    // Add the rendering node to the map< gpu , rendering node >.
    renderingNodes_[ attachedGPU ] = node;

    // Create the TaskRender that wrap the rendering instruction,
    // to be executed concurrently on each device.
    TaskRender *taskRender = new TaskRender( *node  );

    // Add the new task object to
    // the map < rendering node , corresponding rendering task >
    renderingTasks_[ node ] = taskRender;

    // Set the maximum number of active threads of the rendering thread pool and
    // the collector thread pool  to the current number of deployed GPUs.
    rendererPool_.setMaxThreadCount( inUseGPUs_.size( ));
    collectorPool_.setMaxThreadCount( inUseGPUs_.size( ));

    // Map the signal emitted from rendering node after rendering is finished
    // to the corresponding slot.
    connect( node , SIGNAL( finishedRendering( RenderingNode* )),
             this , SLOT( finishedRendering_SLOT( RenderingNode* )));

}

void ParallelRendering::addCompositingNode( const uint64_t gpuIndex )
{

    // handle some errors, TODO: more robust handling for errors.
    if( gpuIndex >= listGPUs_.size() )
        LOG_ERROR("Choose GPU index from [0-%d]", listGPUs_.size() - 1 );

    if( compositingNodeSpecified_ )
    {
        LOG_WARNING("Compositing Node already attached to GPU <%d>" ,
                    compositingNode_->getGPUIndex( ));
        return ;
    }

    // add compositingNode_ that will manage compositing rendered frames.
    compositingNode_ = new CompositingNode( gpuIndex ,
                                            inUseGPUs_.size() ,
                                            frameWidth_ ,
                                            frameHeight_ ,
                                            CompositingNode::Accumulate );


    // Frame index will be assigned to each rendering GPU (rednering node).
    // As a start, consider each frame will be indexed in the next for-loop.
    uint frameIndex = 0 ;

    // for each rendering task finished, a collecting task and a
    // compositing task will follow!
    for( auto renderingDevice : inUseGPUs_ )
    {

        auto renderingNode = renderingNodes_[ renderingDevice ];

        // Now add collectingTasks that transfer buffer from The rendering device
        // (rendering node) to the compositing device (compositing node),
        auto collectingTask =
                new TaskCollect( renderingNode , compositingNode_ ,
                                 frameIndex );

        // Add the collecting task to
        // the map < rendering node , collecting task >
        collectingTasks_[ renderingNode ] = collectingTask ;


        // Add compositing task, that will wrap instructions to accumulate
        // a frame to the collageFrame.
        auto compositingTask =
                new TaskComposite( compositingNode_ , frameIndex ,
                                   compositedFramesCount_ );

        // Add the compositing task to
        // the map < rendering node , compositing task >.
        compositingTasks_[ renderingNode ] = compositingTask ;

        // Map signals from collecting tasks and compositing tasks to the
        // correspondint slots.
        connect( collectingTask ,
                 SIGNAL( frameLoadedToDevice_SIGNAL( RenderingNode* )) ,
                 this , SLOT( frameLoadedToDevice_SLOT( RenderingNode* )));

        connect( compositingTask , SIGNAL(compositingFinished_SIGNAL( )) ,
                 this , SLOT(compositingFinished_SLOT( )));

        frameIndex++ ;
    }


    compositingNodeSpecified_ = true ;
}

void ParallelRendering::distributeBaseVolume1D()
{
    LOG_DEBUG("Distributing Volume");

    // Handle some minor exceptions.
    if( baseVolume_ == nullptr )
        LOG_ERROR( "No Base Volume to distribute!" );

    const int nDevices = inUseGPUs_.size();

    if( nDevices == 0 )
        LOG_ERROR( "No deployed devices to distribute volume!");

    // Decompose the base volume for each rendering device
    // evenly over the X-axis.
    const uint64_t baseXDimension = baseVolume_->getDimensions().x;
    const uint64_t newXDimension =  baseXDimension / nDevices  ;


    //Extract Brick For each node
    for( auto i = 0 ; i < nDevices - 1 ; i++ )
    {
        auto *brick = baseVolume_->getBrick( newXDimension*i ,
                                             newXDimension*( i + 1 ) ,
                                             0,
                                             baseVolume_->getDimensions().y ,
                                             0,
                                             baseVolume_->getDimensions().z );

        bricks_.push_back( brick );
    }

    //The Last node will have the entire remaining brick
    auto brick = baseVolume_->getBrick( newXDimension*( nDevices - 1 ),
                                        baseVolume_->getDimensions().x ,
                                        0,
                                        baseVolume_->getDimensions().y ,
                                        0,
                                        baseVolume_->getDimensions().z );

    bricks_.push_back( brick );
    int i = 0;

    for( auto renderingDevice  : inUseGPUs_ )
    {
        LOG_DEBUG( "Loading subVolume to device" );

        auto subVolume = bricks_[ i++ ];
        renderingNodes_[ renderingDevice ]->loadVolume( subVolume );

        LOG_DEBUG( "[DONE] Loading subVolume to GPU <%d>",
                   renderingNodes_[ renderingDevice ]->getGPUIndex( ));
    }

    // space optimizations:
    // -delete the base volume after distribution
    // -use of memory mapping
}

void ParallelRendering::startRendering()
{
    activeRenderingNodes_ = inUseGPUs_.size();

    LOG_INFO("Triggering Rendering Nodes");

    //First spark of the rendering loop.
    applyTransformation_();

    LOG_INFO("[DONE] Triggering Rendering Nodes");
}

void ParallelRendering::applyTransformation_()
{
    readyPixmapsCount_ = 0 ;

    frameworkProfile.renderingLoopTime_.start();
    frameworkProfile.renderingLoopTimeWithoutPixmap_.start();

    // fetch new transformations if exists.
    syncTransformation_();

    for( auto renderingDevice : inUseGPUs_ )
    {
        auto node = renderingNodes_[ renderingDevice ];

        renderingProfiles[ node ]->threadSpawningTime_.start();
        // Spawn threads and start rendering on each rendering node.
        rendererPool_.start( renderingTasks_[ node ]);
    }

    pendingTransformations_ = false;
    renderingNodesReady_ = false;
}

void ParallelRendering::syncTransformation_()
{
    // modified when no active rendering threads
    translationAsync_ = translation_;
    rotationAsync_ = rotation_;
    brightnessAsync_ = brightness_ ;
    volumeDensityAsync_ = volumeDensity_;
}



RenderingNode &ParallelRendering::getRenderingNode(const uint64_t gpuIndex)
{
    // handle some minor exceptions.
    auto device = listGPUs_.at( gpuIndex );

    if(!( inUseGPUs_.contains( device )))
        LOG_ERROR("No such rendering node!");

    return *renderingNodes_[ device ];

}

uint8_t ParallelRendering::machineGPUsCount() const
{
    return machineGPUsCount_;
}

uint8_t ParallelRendering::activeRenderingNodesCount() const
{
    return activeRenderingNodes_;
}

void ParallelRendering::finishedRendering_SLOT(RenderingNode *finishedNode)
{

    collectingProfiles[ finishedNode ]->threadSpawningTime_.start();

    collectorPool_.start( collectingTasks_[ finishedNode ]);
}

void ParallelRendering::compositingFinished_SLOT()
{

    frameworkProfile.renderingLoopTimeWithoutPixmap_.stop();

    frameworkProfile.convertToPixmapTime_.start();

    QPixmap &finalFrame = compositingNode_->getCollagePixmap();

    frameworkProfile.convertToPixmapTime_.stop();

    frameworkProfile.renderingLoopTime_.stop();

    emit this->finalFrameReady_SIGNAL( finalFrame );

    if( pendingTransformations_ )
        applyTransformation_();
    else
    {
        renderingNodesReady_ = true ;
#ifdef BENCH_MARKING
        static uint testFrames = 0 ;
        if( ++testFrames < TEST_FRAMES )
            updateRotationX_SLOT( rotation_.x + 1 );

        else
            benchmark_();

#endif

    }

}

void ParallelRendering::frameLoadedToDevice_SLOT( RenderingNode *node )
{
//    LOG_DEBUG( "[DONE TRANSFER] from GPU <%d>" , node->getGPUIndex() );

    compositingProfile.threadSpawningTime_.start();
    compositorPool_.start( compositingTasks_[ node ]);

    emit this->frameReady_SIGNAL( node );
}

void ParallelRendering::updateRotationX_SLOT(int angle)
{
    rotation_.x = angle ;
    if( renderingNodesReady_ ) applyTransformation_();
    pendingTransformations_ = true ;
}

void ParallelRendering::updateRotationY_SLOT(int angle)
{
    rotation_.y = angle ;
    if( renderingNodesReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateRotationZ_SLOT(int angle)
{
    rotation_.z = angle ;
    if( renderingNodesReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateTranslationX_SLOT(int distance)
{
    translation_.x = distance;
    if( renderingNodesReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateTranslationY_SLOT(int distance)
{
    translation_.y = distance;
    if( renderingNodesReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateImageBrightness_SLOT(float brightness)
{
    brightness_ = brightness;
    if( renderingNodesReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateVolumeDensity_SLOT(float density)
{
    volumeDensity_ = density;
    if( renderingNodesReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}



void ParallelRendering::benchmark_()
{

    for( auto device : inUseGPUs_ )
    {
        RenderingNode *node = renderingNodes_[ device ];
        uint gpuIndex = node->getGPUIndex();
        RenderingProfile *rProfile = renderingProfiles[ node ];
        CollectingProfile *cProfile = collectingProfiles[ node ];

        printf("Statistics: Rendering on GPU <%d>\n" , gpuIndex );
        rProfile->threadSpawningTime_.print( 1 );
        rProfile->transformationMatrix_.print( 1 );
        rProfile->rendering_.print( 1 );

        printf("Statistics: Data Transfer from GPU <%d> --> Host --> GPU <%d>\n" ,
               gpuIndex , compositingNode_->getGPUIndex() ) ;
        cProfile->threadSpawningTime_.print( 1 );
        cProfile->loadingBufferFromDevice_.print( 1 );
        cProfile->loadingBufferToDevice_.print( 1 );

    }

    printf("Statistics: Compositing on GPU <%d>\n", compositingNode_->getGPUIndex() ) ;
    compositingProfile.threadSpawningTime_.print( 1 );
    //compositingProfile_.setKernelParameters_.print( 1 );
    compositingProfile.accumulatingFrameTime_.print( 1 );
    compositingProfile.loadCollageFromDeviceTime_.print( 1 );
    compositingProfile.rewindCollageTime_.print( 1 );
    compositingProfile.compositingTime_.print( 1 );

    printf("Statistics: framework\n");
    frameworkProfile.renderingLoopTimeWithoutPixmap_.print( 1 );
    frameworkProfile.convertToPixmapTime_.print( 1 );
    frameworkProfile.renderingLoopTime_.print( 1 );

}
