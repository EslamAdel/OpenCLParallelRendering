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




ParallelRendering::ParallelRendering( Volume< uchar > *volume ,
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
    transformation_.translation.x = INITIAL_VOLUME_CENTER_X;
    transformation_.translation.y = INITIAL_VOLUME_CENTER_X;
    transformation_.translation.z = INITIAL_VOLUME_CENTER_Z;

    // Rotation
    transformation_.rotation.x = INITIAL_VOLUME_ROTATION_X;
    transformation_.rotation.y = INITIAL_VOLUME_ROTATION_Y;
    transformation_.rotation.z = INITIAL_VOLUME_ROTATION_Z;

    //Scale
    transformation_.scale.x = INITIAL_VOLUME_SCALE_X;
    transformation_.scale.y = INITIAL_VOLUME_SCALE_Y;
    transformation_.scale.z = INITIAL_VOLUME_SCALE_Z;

    //transfer parameters
    transformation_.transferFunctionOffset = INITIAL_TRANSFER_OFFSET;
    transformation_.transferFunctionScale  = INITIAL_TRANSFER_SCALE;


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

void ParallelRendering::addCLRenderer( const uint64_t gpuIndex )
{
    // if device already occupied by a rendering node, return.
    if( inUseGPUs_.contains( listGPUs_.at( gpuIndex ))) return;


    // pass transformations by reference.
    // translationAsync_, rotationAsync_, volumeDensityAsync_, brightnessAsync_
    // will be accessed by multithreads. So,
    // they should not be modified during the life of the threads.

    CLAbstractRenderer *renderer
            = ( CLAbstractRenderer* )
              new CLRenderer< uchar , float >( gpuIndex,
                                               frameWidth_ ,
                                               frameHeight_ ,
                                               transformationAsync_ );

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


    TaskMakePixmap *taskPixmap = new TaskMakePixmap( );
    taskPixmap->setFrame( renderer->getCLFrame( ).
                          value< CLImage2D< float > *>());
    taskPixmap->setRenderer( renderer );

    makePixmapTasks_[ renderer ] = taskPixmap ;

    // Set the maximum number of active threads of the rendering thread pool and
    // the collector thread pool  to the current number of deployed GPUs.
    rendererPool_.setMaxThreadCount( inUseGPUs_.size( ));
    collectorPool_.setMaxThreadCount( inUseGPUs_.size( ));
    pixmapMakerPool_.setMaxThreadCount( inUseGPUs_.size());

    // Map the signal emitted from rendering node after rendering is finished
    // to the corresponding slot.
    connect( renderer , SIGNAL( finishedRendering( CLAbstractRenderer* )),
             this , SLOT( finishedRendering_SLOT( CLAbstractRenderer* )));

    connect( taskPixmap ,
             SIGNAL(pixmapReady_SIGNAL( QPixmap*,
                                        const CLAbstractRenderer* )) ,
             this , SLOT(pixmapReady_SLOT( QPixmap*,
                                           const CLAbstractRenderer* )));
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

    // If only a single CLRenderer exists, don't use CLCompositor,
    // because compositing is done in a patch using image3d_t
    // while image3d_t doesn't allow occupancy by a single image slice.
    // Instead, use the CLCompositorAccumulate that is based on regular
    // OpenCL buffers.
    if( inUseGPUs_.size() > 1 )
        compositor_ = new CLCompositor< float >( gpuIndex ,
                                                 frameWidth_ ,
                                                 frameHeight_ );

    else
        compositor_ = new CLCompositorAccumulate< float >( gpuIndex ,
                                                           frameWidth_ ,
                                                           frameHeight_ );


    LOG_DEBUG("[DONE] Initialize Compositing Unit");

    finalFramePixmapTask_ = new TaskMakePixmap( );

    connect( finalFramePixmapTask_ ,
             SIGNAL( pixmapReady_SIGNAL( QPixmap* ,
                                         const CLAbstractRenderer* )) ,
             this , SLOT(pixmapReady_SLOT( QPixmap* ,
                                           const CLAbstractRenderer* )));
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
                 SIGNAL( frameLoadedToDevice_SIGNAL( CLAbstractRenderer* )) ,
                 this , SLOT( frameLoadedToDevice_SLOT( CLAbstractRenderer* )));

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
        VolumeVariant volume = VolumeVariant::fromValue( subVolume );

        renderers_[ renderingDevice ]->loadVolume( volume );

        LOG_DEBUG( "[DONE] Loading subVolume to GPU <%d>",
                   renderers_[ renderingDevice ]->getGPUIndex( ));
    }

    emit this->frameworkReady_SIGNAL();
}

void ParallelRendering::distributeBaseVolumeWeighted()
{
    QVector< uint > computingPowerScores ;
    for( const oclHWDL::Device *device : inUseGPUs_ )
    {
        computingPowerScores.append( device->getMaxComputeUnits( ));
    }

    QVector< Volume8 *> bricks =
            baseVolume_->weightedBricking1D( computingPowerScores );

    int i = 0;
    for( auto renderingDevice  : inUseGPUs_ )
    {
        LOG_DEBUG( "Loading subVolume to device" );

        auto subVolume = bricks[ i++ ];
        VolumeVariant volume = VolumeVariant::fromValue( subVolume );

        renderers_[ renderingDevice ]->loadVolume( volume );

        LOG_DEBUG( "[DONE] Loading subVolume to GPU <%d>",
                   renderers_[ renderingDevice ]->getGPUIndex( ));
    }

    emit this->frameworkReady_SIGNAL();

}

void ParallelRendering::distributeBaseVolumeMemoryWeighted()
{

    QVector< uint > memoryScores ;
    for( const oclHWDL::Device *device : inUseGPUs_ )
    {
        memoryScores.append( device->getGlobalMemorySize() / 1024 );
    }

    QVector< Volume8 *> bricks =
            baseVolume_->weightedBricking1D( memoryScores );

    int i = 0;
    for( auto renderingDevice  : inUseGPUs_ )
    {
        LOG_DEBUG( "Loading subVolume to device" );

        auto subVolume = bricks[ i++ ];
        VolumeVariant volume = VolumeVariant::fromValue( subVolume );

        renderers_[ renderingDevice ]->loadVolume( volume );

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

    for( const oclHWDL::Device *renderingDevice : inUseGPUs_ )
    {
        const CLAbstractRenderer *renderer = renderers_.at( renderingDevice );

        TIC( renderingProfiles.at( renderer )->threadSpawning_TIMER );
        // Spawn threads and start rendering on each rendering node.
        rendererPool_.start( renderingTasks_.at( renderer ));
    }

    pendingTransformations_ = false;
    renderersReady_ = false;
}

void ParallelRendering::syncTransformation_()
{
    // modified when no active rendering threads
    transformationAsync_ = transformation_ ;
}


const CLAbstractRenderer &
ParallelRendering::getCLRenderer( const uint64_t gpuIndex ) const
{
    // handle some minor exceptions.
    const oclHWDL::Device *device = listGPUs_.at( gpuIndex );

    if(!( inUseGPUs_.contains( device )))
        LOG_ERROR("No such rendering node!");

    return *renderers_.at( device );

}

const CLAbstractCompositor &ParallelRendering::getCLCompositor() const
{
    return  *compositor_  ;
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

void ParallelRendering::finishedRendering_SLOT( CLAbstractRenderer *renderer )
{
    //    LOG_DEBUG("Finished Rendering");

    TIC( collectingProfiles.at( renderer )->threadSpawning_TIMER );

    collectorPool_.start( collectingTasks_.at( renderer ));

}

void ParallelRendering::compositingFinished_SLOT()
{

    TOC( frameworkProfile.renderingLoop_TIMER );

#ifndef BENCHMARKING
    finalFramePixmapTask_->setFrame(
                compositor_->getFinalFrame().value< CLImage2D< float >*>( ));

    pixmapMakerPool_.start( finalFramePixmapTask_ );
#endif

    if( pendingTransformations_ )
        applyTransformation_();
    else
    {
        renderersReady_ = true ;
#ifdef BENCHMARKING
        static uint framesCounter = 0 ;
        if( ++framesCounter < testFrames )
            updateRotationX_SLOT( transformation_.rotation.x + 1 );

        else
            benchmark_();

#endif

    }


}

void ParallelRendering::frameLoadedToDevice_SLOT( CLAbstractRenderer *renderer )
{
    TIC( compositingProfile.threadSpawning_TIMER );

    //    LOG_DEBUG("Frame[%d] Loaded to device" , node->getFrameIndex( ));

    //accumulate the recently loaded frame to the collage frame.
    compositorPool_.start( compositingTasks_.at( renderer ) );

#ifndef BENCHMARKING
    pixmapMakerPool_.start( makePixmapTasks_.at( renderer ));
#endif

}

void ParallelRendering::pixmapReady_SLOT( QPixmap *pixmap,
                                          const CLAbstractRenderer *renderer )
{
    if( renderer == nullptr )
        emit this->finalFrameReady_SIGNAL( pixmap );
    else
        emit this->frameReady_SIGNAL( pixmap , renderer );
}

void ParallelRendering::updateRotationX_SLOT( int angle )
{
    transformation_.rotation.x = angle ;
    if( renderersReady_ ) applyTransformation_();
    pendingTransformations_ = true ;
}

void ParallelRendering::updateRotationY_SLOT( int angle )
{
    transformation_.rotation.y = angle ;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateRotationZ_SLOT( int angle )
{
    transformation_.rotation.z = angle ;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateTranslationX_SLOT( int distance )
{
    transformation_.translation.x = distance;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateTranslationY_SLOT( int distance )
{
    transformation_.translation.y = distance;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateTranslationZ_SLOT( int distance )
{
    transformation_.translation.z = distance;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateScaleX_SLOT( int distance )
{
    transformation_.scale.x = distance;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateScaleY_SLOT( int distance )
{
    transformation_.scale.y = distance;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateScaleZ_SLOT( int distance )
{
    transformation_.scale.z = distance;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateScaleXYZ_SLOT( int distance )
{
    transformation_.scale.x = distance;
    transformation_.scale.y = distance;
    transformation_.scale.z = distance;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateImageBrightness_SLOT( float brightness )
{
    transformation_.brightness = brightness;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateVolumeDensity_SLOT( float density )
{
    transformation_.volumeDensity = density;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateTransferFunctionScale_SLOT( float scale )
{
    transformation_.transferFunctionScale =scale;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;


}

void ParallelRendering::updateTransferFunctionOffset_SLOT( float offset )
{
    transformation_.transferFunctionOffset = offset ;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;


}

void ParallelRendering::tranferFunctionFlag_SLOT( int flag )
{
    transformation_.transferFunctionFlag = flag ;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}


void ParallelRendering::benchmark_( )
{

    for( auto it : renderers_ )
    {
        CLAbstractRenderer *renderer = it.second ;

        RENDERING_PROFILE_TAG( renderer );
        PRINT( RENDERING_PROFILE( renderer ).threadSpawning_TIMER );
        PRINT( RENDERING_PROFILE( renderer ).mvMatrix_TIMER );
        PRINT( RENDERING_PROFILE( renderer ).rendering_TIMER );

        COLLECTING_PROFILE_TAG( renderer , compositor_ );
        PRINT( COLLECTING_PROFILE( renderer ).threadSpawning_TIMER );
        PRINT( COLLECTING_PROFILE( renderer ).transferingBuffer_TIMER );
    }

    COMPOSITING_PROFILE_TAG( compositor_ );
    //    PRINT( compositingProfile.threadSpawning_TIMER ) ;
    //    PRINT( compositingProfile.accumulatingFrame_TIMER ) ;
    PRINT( compositingProfile.loadFinalFromDevice_TIMER ) ;
    PRINT( compositingProfile.compositing_TIMER ) ;


    FRAMEWORK_PROFILE_TAG( );
    PRINT( frameworkProfile.renderingLoop_TIMER );


    EXIT_PROFILING();
}
