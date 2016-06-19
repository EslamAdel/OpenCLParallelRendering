#include "SortLastRenderer.h"
#include <Logger.h>
#include "Typedefs.hh"
#include "CLFrameVariants.hh"
#include <QList>



namespace clparen {
namespace Parallel{

template< class V , class F >
SortLastRenderer< V , F >::SortLastRenderer(
        Volume< V > *volume ,
        const uint64_t frameWidth ,
        const uint64_t frameHeight ,
        const LoadBalancingMode loadBalancing ,
        const CLData::FRAME_CHANNEL_ORDER channelOrder )
    : baseVolume_( volume ),
      loadBalancingMode_( loadBalancing ) ,
      CLAbstractParallelRenderer( frameWidth , frameHeight , channelOrder )
{

    compositorPool_.setMaxThreadCount( 1 );
}


template< class V , class F >
void SortLastRenderer< V , F >::addCLRenderer( const uint64_t gpuIndex )
{
    // if device already occupied by a rendering node, return.
    if( inUseGPUs_.contains( listGPUs_.at( gpuIndex ))) return;


    // pass transformations by reference.
    // translationAsync_, rotationAsync_, volumeDensityAsync_, brightnessAsync_
    // will be accessed by multithreads. So,
    // they should not be modified during the life of the threads.

    Renderer::CLAbstractRenderer *renderer =
            new Renderer::CLRenderer< V , F >(
                gpuIndex, transformationAsync_ ,
                Dimensions2D( frameWidth_ , frameHeight_ ) ,
                frameChannelOrder_ );

    ATTACH_RENDERING_PROFILE( renderer );
    ATTACH_COLLECTING_PROFILE( renderer );

    auto attachedGPU = listGPUs_.at( gpuIndex );

    // Add the recently attached GPU to Set of usedGPUs.
    inUseGPUs_ << attachedGPU;

    // Add the rendering node to the map< gpu , rendering node >.
    renderers_[ attachedGPU ] = renderer;

    // Create the TaskRender that wrap the rendering instruction,
    // to be executed concurrently on each device.
    Renderer::Task::TaskRender *taskRender =
            new Renderer::Task::TaskRender( *renderer  );

    // Add the new task object to
    // the map < rendering node , corresponding rendering task >
    renderingTasks_[ renderer ] = taskRender;


    Task::TaskMakePixmap *taskPixmap = new Task::TaskMakePixmap( );
    taskPixmap->setFrame( renderer->getCLFrame( ).
                          value< CLData::CLImage2D< F > *>());
    taskPixmap->setRenderer( renderer );

    makePixmapTasks_[ renderer ] = taskPixmap ;

    // Set the maximum number of active threads of the rendering thread pool and
    // the collector thread pool  to the current number of deployed GPUs.
    rendererPool_.setMaxThreadCount( inUseGPUs_.size( ));
    collectorPool_.setMaxThreadCount( inUseGPUs_.size( ));
    pixmapMakerPool_.setMaxThreadCount( inUseGPUs_.size());

    // Map the signal emitted from rendering node after rendering is finished
    // to the corresponding slot.
    connect( renderer ,
             SIGNAL( finishedRendering( Renderer::CLAbstractRenderer* )),
             this ,
             SLOT( finishedRendering_SLOT( Renderer::CLAbstractRenderer* )));

    connect( taskPixmap ,
             SIGNAL(pixmapReady_SIGNAL(
                        QPixmap*,
                        const Renderer::CLAbstractRenderer* )) ,
             this ,
             SLOT(pixmapReady_SLOT(
                      QPixmap*,
                      const Renderer::CLAbstractRenderer* )));
}



template< class V , class F >
void SortLastRenderer< V , F >::addCLCompositor( const uint64_t gpuIndex )
{
    LOG_INFO("Linking Rendering Units with Compositing Unit...");

    if( gpuIndex >= listGPUs_.size() )
        LOG_ERROR("Choose GPU index from [0-%d]", listGPUs_.size() - 1 );

    LOG_DEBUG("Initialize Compositor");

    // If multiple renderers deployed, use compositor.
    // Otherwise, no compositing to be performed.
    if( inUseGPUs_.size() > 1 )
        compositor_ = new Compositor::CLCompositor< F >( gpuIndex ,
                                                         frameWidth_ ,
                                                         frameHeight_ );


    LOG_DEBUG("[DONE] Initialize Compositor");

    finalFramePixmapTask_ = new Task::TaskMakePixmap( );
    connect( finalFramePixmapTask_ ,
             SIGNAL( pixmapReady_SIGNAL(
                         QPixmap* ,
                         const Renderer::CLAbstractRenderer* )) ,
             this ,
             SLOT(pixmapReady_SLOT(
                      QPixmap* ,
                      const Renderer::CLAbstractRenderer* )));

    // for each rendering task finished, a collecting task and a
    // compositing task will be assigned!
    if( inUseGPUs_.size() > 1 )
        for( auto renderingDevice : inUseGPUs_ )
        {
            auto renderer = renderers_[ renderingDevice ];

            LOG_DEBUG("Connecting CLRenderer< %d >" ,
                      renderer->getGPUIndex( ));

            //register a frame to be allocated in the compositor device.
            compositor_->allocateFrame( renderer );

            Compositor::Task::TaskComposite *compositingTask =
                    new Compositor::Task::TaskComposite( compositor_ ,
                                                         renderer );

            compositingTasks_[ renderer ] = compositingTask ;

            // Now add collectingTasks that transfer buffer from The rendering device
            // (rendering node) to the compositing device (compositing node),
            auto collectingTask =
                    new Task::TaskCollect( renderer , compositor_ );

            // Add the collecting task to
            // the map < rendering node , collecting task >
            collectingTasks_[ renderer ] = collectingTask ;


            // Map signals from collecting tasks and compositing tasks to the
            // correspondint slots.
            connect( collectingTask ,
                     SIGNAL( frameLoadedToDevice_SIGNAL(
                                 Renderer::CLAbstractRenderer* )) ,
                     this , SLOT( frameLoadedToDevice_SLOT(
                                      Renderer::CLAbstractRenderer* )));

            connect( compositingTask , SIGNAL( compositingFinished_SIGNAL( )) ,
                     this , SLOT( compositingFinished_SLOT( )));
        }


    LOG_INFO("[DONE] Linking Rendering Units with Compositing Unit...");
}


template< class V , class F >
void SortLastRenderer< V , F >::distributeBaseVolume1D()
{
    const int nDevices = inUseGPUs_.size();

    if( nDevices == 0 )
        LOG_ERROR( "No deployed devices to distribute volume!");

    //QVector< Volume8 *> bricks = baseVolume_->getBricksXAxis( nDevices );

    QVector< Volume< V > *> bricks = baseVolume_->heuristicBricking( nDevices );
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

template< class V , class F >
void SortLastRenderer< V , F >::distributeBaseVolumeWeighted()
{
    QVector< uint > computingPowerScores ;
    for( const oclHWDL::Device *device : inUseGPUs_ )
    {
        computingPowerScores.append( device->getMaxComputeUnits( ));
    }

    QVector< BrickParameters< V > > bricks =
            baseVolume_->weightedBricking1D( computingPowerScores );

    int i = 0;
    for( auto renderingDevice  : inUseGPUs_ )
    {
        LOG_DEBUG( "Loading subVolume to device" );

        Volume< V > *subVolume = new Volume< V >( bricks[ i++ ]);
        VolumeVariant volume = VolumeVariant::fromValue( subVolume );

        renderers_[ renderingDevice ]->loadVolume( volume );

        LOG_DEBUG( "[DONE] Loading subVolume to GPU <%d>",
                   renderers_[ renderingDevice ]->getGPUIndex( ));
    }

    emit this->frameworkReady_SIGNAL();

}

template< class V , class F >
void SortLastRenderer< V , F >::distributeBaseVolumeMemoryWeighted()
{

    QVector< uint > memoryScores ;
    for( const oclHWDL::Device *device : inUseGPUs_ )
    {
        memoryScores.append( device->getGlobalMemorySize() / 1024 );
    }

    QVector< BrickParameters< V > > bricks =
            baseVolume_->weightedBricking1D( memoryScores );

    int i = 0;
    for( auto renderingDevice  : inUseGPUs_ )
    {
        LOG_DEBUG( "Loading subVolume to device" );

        Volume< V > *subVolume = new Volume< V >( bricks[ i++ ]);
        VolumeVariant volume = VolumeVariant::fromValue( subVolume );

        renderers_[ renderingDevice ]->loadVolume( volume );

        LOG_DEBUG( "[DONE] Loading subVolume to GPU <%d>",
                   renderers_[ renderingDevice ]->getGPUIndex( ));
    }

    emit this->frameworkReady_SIGNAL();
}


template< class V , class F >
void SortLastRenderer< V , F >::finishedRendering_SLOT(
        Renderer::CLAbstractRenderer *renderer )
{
    //    LOG_DEBUG("Finished Rendering");

    if( renderers_.size() > 1 )
    {
        TIC( collectingProfiles.value( renderer )->threadSpawning_TIMER );
        collectorPool_.start( collectingTasks_[ renderer ]);
    }
    else
    {
        CLData::CLImage2D< F > *rendererdFrame =
                renderer->getCLFrame().value< CLData::CLImage2D< F > *>( );

        rendererdFrame->readDeviceData( renderer->getCommandQueue() ,
                                        CL_TRUE );
        QPixmap &pixmap = rendererdFrame->getFramePixmap();

        emit this->finalFrameReady_SIGNAL( &pixmap );

        renderersReady_ = true ;

    }

}

template< class V , class F >
void SortLastRenderer< V , F >::compositingFinished_SLOT()
{

    TOC( frameworkProfile.renderingLoop_TIMER );

#ifndef BENCHMARKING
    finalFramePixmapTask_->setFrame(
                compositor_->getFinalFrame().value< CLData::CLImage2D< F >*>( ));

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

template< class V , class F >
void SortLastRenderer< V , F >::frameLoadedToDevice_SLOT(
        Renderer::CLAbstractRenderer *renderer )
{
    TIC( compositingProfile.threadSpawning_TIMER );

    //    LOG_DEBUG("Frame[%d] Loaded to device" , node->getFrameIndex( ));

    //accumulate the recently loaded frame to the collage frame.
    compositorPool_.start( compositingTasks_[ renderer ]);

#ifndef BENCHMARKING
    pixmapMakerPool_.start( makePixmapTasks_[ renderer ]);
#endif

}

template< class V , class F >
void SortLastRenderer< V , F >::pixmapReady_SLOT(
        QPixmap *pixmap,
        const Renderer::CLAbstractRenderer *renderer )
{
    if( renderer == nullptr )
        emit this->finalFrameReady_SIGNAL( pixmap );
    else
        emit this->frameReady_SIGNAL( pixmap , renderer );
}


template< class V , class F >
void SortLastRenderer< V , F >::distributeBaseVolume()
{

    switch( loadBalancingMode_ )
    {
    case LoadBalancingMode::LOAD_BALANCING_COMPUTATIONAL :
        distributeBaseVolumeWeighted();
        break;

    case LoadBalancingMode::LOAD_BALANCING_MEMORY :
        distributeBaseVolumeMemoryWeighted();
        break;

    case LoadBalancingMode::LOAD_BALANCING_OFF:
        distributeBaseVolume1D();
        break;

    default:
        LOG_ERROR("Unexpected value!");
    }

}

template< class V , class F >
void SortLastRenderer< V , F >::initializeRenderers()
{

}


}
}


#include "SortLastRenderer.ipp"
