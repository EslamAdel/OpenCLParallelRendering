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

    clCompositor_ = 0;
    compositorPool_.setMaxThreadCount( 1 );

}


template< class V , class F >
void SortLastRenderer< V , F >::addCLRenderer( const uint64_t gpuIndex )
{
    // if device already occupied by a rendering node, return.
    if( clRenderers_.keys().contains( gpuIndex )) return;


    // pass transformations by reference.
    // translationAsync_, rotationAsync_, volumeDensityAsync_, brightnessAsync_
    // will be accessed by multithreads. So,
    // they should not be modified during the life of the threads.

    Renderer::CLRenderer< V , F > *renderer =
            new Renderer::CLRenderer< V , F >(
                gpuIndex , transformationAsync_ ,
                Dimensions2D( frameWidth_ , frameHeight_ ) ,
                frameChannelOrder_ );

    ATTACH_RENDERING_PROFILE( renderer );
    ATTACH_COLLECTING_PROFILE( renderer );


    // Add the rendering node to the map< gpu , rendering node >.
    clRenderers_[ gpuIndex ] = renderer;
    renderers_[ gpuIndex ] = renderer;


    // Set the maximum number of active threads of the rendering thread pool and
    // the collector thread pool  to the current number of deployed GPUs.
    rendererPool_.setMaxThreadCount( clRenderers_.size( ));
    collectorPool_.setMaxThreadCount( clRenderers_.size( ));

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
    if( clRenderers_.size() > 1 )
    {
        clCompositor_ = new Compositor::CLCompositor< F >( gpuIndex ,
                                                           frameWidth_ ,
                                                           frameHeight_ );
        compositor_ = clCompositor_;
    }


    LOG_DEBUG("[DONE] Initialize Compositor");


    // for each rendering task finished, a collecting task and a
    // compositing task will be assigned!
    if( clRenderers_.size() > 1 )
        for( Renderer::CLRenderer< V , F > *renderer : clRenderers_ )
        {
            LOG_DEBUG("Connecting CLRenderer< %d >" ,
                      renderer->getGPUIndex( ));

            //register a frame to be allocated in the compositor device.
            clCompositor_->allocateFrame( renderer );

        }




    LOG_INFO("[DONE] Linking Rendering Units with Compositing Unit...");
}


template< class V , class F >
void SortLastRenderer< V , F >::distributeBaseVolume1D()
{
    const int nDevices = clRenderers_.size();

    if( nDevices == 0 )
        LOG_ERROR( "No deployed devices to distribute volume!");

    //QVector< Volume8 *> bricks = baseVolume_->getBricksXAxis( nDevices );

    QVector< Volume< V > *> bricks = baseVolume_->heuristicBricking( nDevices );
    int i = 0;

    for( Renderer::CLRenderer< V , F > *renderer : clRenderers_ )
    {
        LOG_DEBUG( "Loading subVolume to device" );

        auto subVolume = bricks[ i++ ];
        renderer->loadVolume( subVolume );

        LOG_DEBUG( "[DONE] Loading subVolume to GPU <%d>",
                   renderer->getGPUIndex( ));
    }

}

template< class V , class F >
void SortLastRenderer< V , F >::distributeBaseVolumeWeighted()
{

    QVector< uint > computingPowerScores ;
    for( const uint gpuIdnex : clRenderers_.keys( ))
        computingPowerScores.append(
                    listGPUs_[ gpuIdnex ]->getMaxComputeUnits( ));


    QVector< BrickParameters< V > > bricks =
            baseVolume_->weightedBricking1D( computingPowerScores );

    int i = 0;
    for( Renderer::CLRenderer< V , F > *renderer : clRenderers_ )
    {
        LOG_DEBUG( "Loading subVolume to device" );

        Volume< V > *subVolume = new Volume< V >( bricks[ i++ ]);

        renderer->loadVolume( subVolume );

        LOG_DEBUG( "[DONE] Loading subVolume to GPU <%d>",
                   renderer->getGPUIndex( ));
    }


}

template< class V , class F >
void SortLastRenderer< V , F >::distributeBaseVolumeMemoryWeighted()
{

    QVector< uint > memoryScores ;
    for( const uint gpuIndex : clRenderers_.keys( ))
        memoryScores.append( listGPUs_[ gpuIndex ]->getGlobalMemorySize() / 1024 );


    QVector< BrickParameters< V > > bricks =
            baseVolume_->weightedBricking1D( memoryScores );

    int i = 0;
    for( Renderer::CLRenderer< V , F > *renderer : clRenderers_ )
    {
        LOG_DEBUG( "Loading subVolume to device" );

        Volume< V > *subVolume = new Volume< V >( bricks[ i++ ]);
        renderer->loadVolume( subVolume );

        LOG_DEBUG( "[DONE] Loading subVolume to GPU <%d>",
                   renderer->getGPUIndex( ));
    }

}

template< class V , class F >
void SortLastRenderer< V , F >::applyTransformation_()
{

    assertThread_();
    TIC( frameworkProfile.renderingLoop_TIMER );

    // fetch new transformations if exists.
    syncTransformation_();

    for( uint gpuIndex : clRenderers_.keys( ))
    {

        TIC( renderingProfiles.value( clRenderers_[ gpuIndex ] )->threadSpawning_TIMER );
        // Spawn threads and start rendering on each rendering node.
        QtConcurrent::run( &rendererPool_ , this ,
                           &SortLastRenderer::render_ ,
                           gpuIndex );
    }

    pendingTransformations_ = false;
    renderersReady_ = false;
}

template< class V , class F >
void SortLastRenderer< V , F >::benchmark_()
{
    for( Renderer::CLRenderer< V , F > *renderer : clRenderers_ )
    {
        RENDERING_PROFILE_TAG( renderer );
        PRINT( RENDERING_PROFILE( renderer ).threadSpawning_TIMER );
        PRINT( RENDERING_PROFILE( renderer ).mvMatrix_TIMER );
        PRINT( RENDERING_PROFILE( renderer ).rendering_TIMER );

        COLLECTING_PROFILE_TAG( renderer , compositor_ );
        PRINT( COLLECTING_PROFILE( renderer ).threadSpawning_TIMER );
        PRINT( COLLECTING_PROFILE( renderer ).transferingBuffer_TIMER );
    }

    COMPOSITING_PROFILE_TAG( compositor_ );
    PRINT( compositingProfile.loadFinalFromDevice_TIMER ) ;
    PRINT( compositingProfile.compositing_TIMER ) ;


    FRAMEWORK_PROFILE_TAG( );
    PRINT( frameworkProfile.renderingLoop_TIMER );


    EXIT_PROFILING();
}


template< class V , class F >
void SortLastRenderer< V , F >::finishedRendering_SLOT(
        uint gpuIndex )
{

    assertThread_();

    if( clRenderers_.size() > 1 )
    {
        LOG_DEBUG("Finished Rendering< %d >", gpuIndex);

//        TIC( collectingProfiles.value( clRenderers_[ gpuIndex ] )->threadSpawning_TIMER );
//        collectorPool_.start( collectingTasks_[ gpuIndex ]);

        QtConcurrent::run( &collectorPool_ , this ,
                           &SortLastRenderer::collectFrame_ ,
                           gpuIndex );
    }
    else
    {
        clRenderers_[ gpuIndex ]->downloadFrame();

        Q_EMIT finalFrameReady_SIGNAL(
                    &clRenderers_[ gpuIndex ]->finalPixmap( ));

        renderersReady_ = true ;
    }

}

template< class V , class F >
void SortLastRenderer< V , F >::compositingFinished_SLOT()
{

    assertThread_();

    LOG_DEBUG("Finished compositing");

    TOC( frameworkProfile.renderingLoop_TIMER );

#ifndef BENCHMARKING
    Q_EMIT finalFrameReady_SIGNAL( & clCompositor_->getCLImage2D().getFramePixmap());
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
        uint gpuIndex )
{

    assertThread_();

    LOG_DEBUG("Frame[%d] Loaded to device" , gpuIndex );

    //accumulate the recently loaded frame to the final frame.
    QtConcurrent::run( &compositorPool_ , this ,
                       &SortLastRenderer::composite_ ,
                       gpuIndex );

#ifndef BENCHMARKING    
    QtConcurrent::run( this , &SortLastRenderer< V , F >::makePixmap_ ,
                       gpuIndex );
#endif

}

template< class V , class F >
void SortLastRenderer< V , F >::pixmapReady_SLOT(
        QPixmap *pixmap,
        uint gpuIndex )
{
    assertThread_();


    Q_EMIT this->frameReady_SIGNAL( pixmap , gpuIndex );
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

    Q_EMIT this->frameworkReady_SIGNAL();


}

template< class V , class F >
void SortLastRenderer< V , F >::initializeRenderers()
{

}

template< class V , class F >
void SortLastRenderer< V , F >::makePixmap_( uint gpuIndex )
{
    QPixmap &pixmap = clRenderers_[ gpuIndex ]->finalPixmap();

    Q_EMIT frameReady_SIGNAL( &pixmap , gpuIndex );
}


template< class V , class F >
void SortLastRenderer< V , F >::composite_( uint )
{

//    QMutexLocker lock( &compositingMutex_ );

    LOG_DEBUG("Compositing");
    compositor_->composite( );

    if( compositor_->readOutReady( ))
    {
        TIC( compositingProfile.loadFinalFromDevice_TIMER );
        compositor_->loadFinalFrame( );
        TOC( compositingProfile.loadFinalFromDevice_TIMER );
        Q_EMIT compositingFinished_SIGNAL( );

        LOG_DEBUG("[DONE] Compositing");
    }
}

template< class V , class F >
void SortLastRenderer< V , F >::render_(
        uint gpuIndex )
{
    renderers_[ gpuIndex ]->applyTransformation();

    Q_EMIT finishedRendering_SIGNAL( gpuIndex );
}


template< class V , class F >
void SortLastRenderer< V , F >::collectFrame_( uint gpuIndex )
{

    LOG_DEBUG("Transfering frame<%d> to compositor", gpuIndex);


    //upload frame from rendering GPU to HOST.
    TIC( COLLECTING_PROFILE( renderers_[ gpuIndex ] ).transferingBuffer_TIMER );

    clCompositor_->collectFrame( renderers_[ gpuIndex ] ,
                                 CL_TRUE );

    LOG_DEBUG("[DONE] Transfering frame<%d> to compositor", gpuIndex);
    TOC( COLLECTING_PROFILE( renderers_[ gpuIndex ] ).transferingBuffer_TIMER ) ;


    Q_EMIT frameLoadedToDevice_SIGNAL( gpuIndex );
}

}
}


#include "SortLastRenderer.ipp"
