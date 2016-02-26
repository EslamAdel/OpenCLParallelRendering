#include "ParallelRendering.h"
#include <Logger.h>
#include "Typedefs.hh"
#include <QList>

#define VOLUME_PREFIX "/projects/volume-datasets/foot/foot"
#define INITIAL_VOLUME_CENTER_X 0.0
#define INITIAL_VOLUME_CENTER_Y 0.0
#define INITIAL_VOLUME_CENTER_Z 0.0
#define INITIAL_VOLUME_ROTATION_X 0.0
#define INITIAL_VOLUME_ROTATION_Y 0.0
#define INITIAL_VOLUME_ROTATION_Z 0.0
#define INITIAL_VOLUME_SCALE_X 1.0
#define INITIAL_VOLUME_SCALE_Y 1.0
#define INITIAL_VOLUME_SCALE_Z 1.0
#define INITIAL_TRANSFER_SCALE    1.0
#define INITIAL_TRANSFER_OFFSET   0.0




#ifdef BENCHMARKING
//Profiles Difinitions
DEFINE_PROFILES

#define AUTO_ROTATE

#endif




ParallelRendering::ParallelRendering( Volume<uchar> *volume ,
                                      const uint frameWidth ,
                                      const uint frameHeight )
    : baseVolume_( volume ),
      frameWidth_( frameWidth ),
      frameHeight_( frameHeight ),
      renderersReady_( false ),
      compositorSpecified_( false ),
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

    //Scale
    scale_.x = INITIAL_VOLUME_SCALE_X;
    scale_.y = INITIAL_VOLUME_SCALE_Y;
    scale_.z = INITIAL_VOLUME_SCALE_Z;

    //transfer parameters
    transferFunctionOffset_= INITIAL_TRANSFER_OFFSET;
    transferFunctionScale_ = INITIAL_TRANSFER_SCALE;


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
        addCLRenderer( gpuIndex );

}

void ParallelRendering::addCLRenderer( const uint64_t gpuIndex)
{
    // if device already occupied by a rendering node, return.
    if( inUseGPUs_.contains( listGPUs_.at( gpuIndex ))) return;


    // pass transformations by reference.
    // translationAsync_, rotationAsync_, volumeDensityAsync_, brightnessAsync_
    // will be accessed by multithreads. So,
    // they should not be modified during the life of the threads.

    CLRenderer *renderer = new CLRenderer( gpuIndex,
                                           frameWidth_ ,
                                           frameHeight_ ,
                                           translationAsync_,
                                           rotationAsync_,
                                           scaleAsync_,
                                           volumeDensityAsync_,
                                           brightnessAsync_,
                                           transferFunctionScaleAsync_,
                                           transferFunctionOffsetAsync_);

    ATTACH_RENDERING_PROFILE( renderer );
    ATTACH_COLLECTING_PROFILE( renderer );

    auto attachedGPU = listGPUs_.at( gpuIndex );

    // Add the recently attached GPU to Set of usedGPUs.
    inUseGPUs_ << attachedGPU;

    // Add the rendering node to the map< gpu , rendering node >.
    renderers_[ attachedGPU ] = renderer;

    // Create the TaskRender that wrap the rendering instruction,
    // to be executed concurrently on each device.
    TaskRender *taskRender = new TaskRender( *renderer  );

    // Add the new task object to
    // the map < rendering node , corresponding rendering task >
    renderingTasks_[ renderer ] = taskRender;


    TaskMakePixmap *taskPixmap = new TaskMakePixmap( renderer->getCLFrame() ,
                                                     renderer );
    makePixmapTasks_[ renderer ] = taskPixmap ;

    // Set the maximum number of active threads of the rendering thread pool and
    // the collector thread pool  to the current number of deployed GPUs.
    rendererPool_.setMaxThreadCount( inUseGPUs_.size( ));
    collectorPool_.setMaxThreadCount( inUseGPUs_.size( ));
    pixmapMakerPool_.setMaxThreadCount( inUseGPUs_.size());

    // Map the signal emitted from rendering node after rendering is finished
    // to the corresponding slot.
    connect( renderer , SIGNAL( finishedRendering( CLRenderer* )),
             this , SLOT( finishedRendering_SLOT( CLRenderer* )));

    connect( taskPixmap ,
             SIGNAL(pixmapReady_SIGNAL(QPixmap*,const CLRenderer*)) ,
             this , SLOT(pixmapReady_SLOT(QPixmap*,const CLRenderer*)));
}

int ParallelRendering::getCLRenderersCount() const
{
    return inUseGPUs_.size();
}


void ParallelRendering::addCLCompositor( const uint64_t gpuIndex )
{
    LOG_INFO("Linking Rendering Units with Compositing Unit...");

    // handle some errors, TODO: more robust handling for errors.
    if( gpuIndex >= listGPUs_.size() )
        LOG_ERROR("Choose GPU index from [0-%d]", listGPUs_.size() - 1 );

    if( compositorSpecified_ )
    {
        LOG_WARNING("Compositing Node already attached to GPU <%d>" ,
                    compositor_->getGPUIndex( ));
        return ;
    }

    LOG_DEBUG("Initialize Compositing Unit");
    compositor_ = new CLCompositor( gpuIndex ,
                                    frameWidth_ ,
                                    frameHeight_ );



    LOG_DEBUG("[DONE] Initialize Compositing Unit");

    collagePixmapTask_ =
            new TaskMakePixmap( compositor_->getCLFrameCollage() );

    connect( collagePixmapTask_ ,
             SIGNAL( pixmapReady_SIGNAL( QPixmap* , const CLRenderer* )) ,
             this , SLOT(pixmapReady_SLOT( QPixmap* , const CLRenderer* )));
    // Frame index will be assigned to each rendering GPU (rednering node).
    // As a start, consider each frame will be indexed in the next for-loop.






    uint frameIndex = 0 ;

    // for each rendering task finished, a collecting task and a
    // compositing task will follow!
    for( auto renderingDevice : inUseGPUs_ )
    {
        auto renderer = renderers_[ renderingDevice ];

        LOG_DEBUG("Connecting CLRenderer< %d >" ,
                  renderer->getGPUIndex( ));

        renderer->setFrameIndex( frameIndex );

        //register a frame to be allocated in the compositor device.
        compositor_->allocateFrame( renderer );

        TaskComposite *compositingTask =
                new TaskComposite( compositor_ ,
                                   renderer );

        compositingTasks_[ renderer ] = compositingTask ;

        // Now add collectingTasks that transfer buffer from The rendering device
        // (rendering node) to the compositing device (compositing node),
        auto collectingTask =
                new TaskCollect( renderer , compositor_ );

        // Add the collecting task to
        // the map < rendering node , collecting task >
        collectingTasks_[ renderer ] = collectingTask ;


        // Map signals from collecting tasks and compositing tasks to the
        // correspondint slots.
        connect( collectingTask ,
                 SIGNAL( frameLoadedToDevice_SIGNAL( CLRenderer* )) ,
                 this , SLOT( frameLoadedToDevice_SLOT( CLRenderer* )));

        connect( compositingTask , SIGNAL( compositingFinished_SIGNAL( )) ,
                 this , SLOT( compositingFinished_SLOT( )));

        frameIndex++ ;
    }


    compositorSpecified_ = true ;

    LOG_INFO("[DONE] Linking Rendering Units with Compositing Unit...");

}


void ParallelRendering::distributeBaseVolume1D()
{
    const int nDevices = inUseGPUs_.size();

    if( nDevices == 0 )
        LOG_ERROR( "No deployed devices to distribute volume!");

    //QVector< Volume8 *> bricks = baseVolume_->getBricksXAxis( nDevices );

    QVector< Volume8 *> bricks = baseVolume_->heuristicBricking( nDevices );
    int i = 0;

    for( auto renderingDevice  : inUseGPUs_ )
    {
        LOG_DEBUG( "Loading subVolume to device" );

        auto subVolume = bricks[ i++ ];
        renderers_[ renderingDevice ]->loadVolume( subVolume );

        LOG_DEBUG( "[DONE] Loading subVolume to GPU <%d>",
                   renderers_[ renderingDevice ]->getGPUIndex( ));
    }

    emit this->frameworkReady_SIGNAL();
}



void ParallelRendering::startRendering()
{
    activeRenderers_ =  inUseGPUs_.size();

    LOG_INFO("Triggering Rendering Nodes");

    //First spark of the rendering loop.
    applyTransformation_();

    LOG_INFO("[DONE] Triggering Rendering Nodes");
}

void ParallelRendering::applyTransformation_()
{
    readyPixmapsCount_ = 0 ;

    TIC( frameworkProfile.renderingLoop_TIMER );

    // fetch new transformations if exists.
    syncTransformation_();

    for( auto renderingDevice : inUseGPUs_ )
    {
        auto renderer = renderers_[ renderingDevice ];

        TIC( renderingProfiles[ renderer ]->threadSpawning_TIMER );
        // Spawn threads and start rendering on each rendering node.
        rendererPool_.start( renderingTasks_[ renderer ]);
    }

    pendingTransformations_ = false;
    renderersReady_ = false;
}

void ParallelRendering::syncTransformation_()
{
    // modified when no active rendering threads
    translationAsync_ = translation_;
    rotationAsync_ = rotation_;
    scaleAsync_ = scale_;
    brightnessAsync_ = brightness_ ;
    volumeDensityAsync_ = volumeDensity_;
    transferFunctionScaleAsync_=transferFunctionScale_;
    transferFunctionOffsetAsync_=transferFunctionOffset_;

}


CLRenderer &ParallelRendering::getCLRenderer( const uint64_t gpuIndex )
{
    // handle some minor exceptions.
    auto device = listGPUs_.at( gpuIndex );

    if(!( inUseGPUs_.contains( device )))
        LOG_ERROR("No such rendering node!");

    return *renderers_[ device ];

}

CLCompositor &ParallelRendering::getCLCompositor()
{
    return *compositor_ ;
}


uint ParallelRendering::getMachineGPUsCount() const
{
    return machineGPUsCount_;
}

uint ParallelRendering::getFrameWidth() const
{
    return frameWidth_;
}

uint ParallelRendering::getFrameHeight() const
{
    return frameHeight_;
}

void ParallelRendering::finishedRendering_SLOT(CLRenderer *renderer )
{
    //    LOG_DEBUG("Finished Rendering");

    TIC( collectingProfiles[ renderer ]->threadSpawning_TIMER );

    collectorPool_.start( collectingTasks_[ renderer ]);

}

void ParallelRendering::compositingFinished_SLOT()
{


    TOC( frameworkProfile.renderingLoop_TIMER );

#ifndef BENCHMARKING
    pixmapMakerPool_.start( collagePixmapTask_ );
#endif

    if( pendingTransformations_ )
        applyTransformation_();
    else
    {
        renderersReady_ = true ;
#ifdef BENCHMARKING
        static uint framesCounter = 0 ;
        if( ++framesCounter < testFrames )
            updateRotationX_SLOT( rotation_.x + 1 );

        else
            benchmark_();

#endif

    }


}

void ParallelRendering::frameLoadedToDevice_SLOT( CLRenderer *renderer )
{
    TIC( compositingProfile.threadSpawning_TIMER );

    //    LOG_DEBUG("Frame[%d] Loaded to device" , node->getFrameIndex( ));

    //accumulate the recently loaded frame to the collage frame.
    compositorPool_.start( compositingTasks_[ renderer ] );

#ifndef BENCHMARKING
    pixmapMakerPool_.start( makePixmapTasks_[ renderer ]);
#endif

}

void ParallelRendering::pixmapReady_SLOT( QPixmap *pixmap,
                                          const CLRenderer *renderer )
{

    if( renderer == nullptr )
        emit this->finalFrameReady_SIGNAL( pixmap );
    else
        emit this->frameReady_SIGNAL( pixmap , renderer );
}

void ParallelRendering::updateRotationX_SLOT(int angle)
{
    rotation_.x = angle ;
    if( renderersReady_ ) applyTransformation_();
    pendingTransformations_ = true ;
}

void ParallelRendering::updateRotationY_SLOT(int angle)
{
    rotation_.y = angle ;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateRotationZ_SLOT(int angle)
{
    rotation_.z = angle ;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateTranslationX_SLOT(int distance)
{
    translation_.x = distance;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateTranslationY_SLOT(int distance)
{
    translation_.y = distance;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateTranslationZ_SLOT(int distance)
{
    translation_.z = distance;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateScaleX_SLOT(int distance)
{
    scale_.x = distance;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateScaleY_SLOT(int distance)
{
    scale_.y = distance;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateScaleZ_SLOT(int distance)
{
    scale_.z = distance;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateScaleXYZ_SLOT(int distance)
{
    scale_.x = distance;
    scale_.y = distance;
    scale_.z = distance;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateImageBrightness_SLOT(float brightness)
{
    brightness_ = brightness;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateVolumeDensity_SLOT(float density)
{
    volumeDensity_ = density;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateTransferFunctionScale_SLOT(float scale)
{
    transferFunctionScale_=scale;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;


}

void ParallelRendering::updateTransferFunctionOffset_SLOT(float offset)
{
    transferFunctionOffset_=offset;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;


}

void ParallelRendering::tranferFunctionFlag_SLOT(int flag)
{
    for( auto renderingDevice : inUseGPUs_ )
    {
        CLRenderer* renderer = renderers_[ renderingDevice ];
        renderer->setTransferFunctionFlag(flag);
    }
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}


void ParallelRendering::benchmark_()
{

    for( auto it : renderers_ )
    {
        CLRenderer *renderer = it.second ;

        RENDERING_PROFILE_TAG( renderer );
        PRINT( RENDERING_PROFILE( renderer ).threadSpawning_TIMER );
        PRINT( RENDERING_PROFILE( renderer ).mvMatrix_TIMER );
        PRINT( RENDERING_PROFILE( renderer ).rendering_TIMER );

        COLLECTING_PROFILE_TAG( renderer , compositor_ );
        PRINT( COLLECTING_PROFILE( renderer ).threadSpawning_TIMER );
        PRINT( COLLECTING_PROFILE( renderer ).transferingBuffer_TIMER );
    }

    COMPOSITING_PROFILE_TAG( compositor_ );
    PRINT( compositingProfile.threadSpawning_TIMER ) ;
    PRINT( compositingProfile.accumulatingFrame_TIMER ) ;
    PRINT( compositingProfile.loadCollageFromDevice_TIMER ) ;
    PRINT( compositingProfile.compositing_TIMER ) ;


    FRAMEWORK_PROFILE_TAG( );
    PRINT( frameworkProfile.renderingLoop_TIMER );


    EXIT_PROFILING();
}
