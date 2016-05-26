#include "CLAbstractParallelRenderer.h"
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


namespace clparen
{
namespace Parallel
{

CLAbstractParallelRenderer::CLAbstractParallelRenderer(
        const uint64_t frameWidth,
        const uint64_t frameHeight,
        QObject *parent )
    : QObject( parent ) ,
      frameWidth_( frameWidth ) ,
      frameHeight_( frameHeight ),
      renderersReady_( false ),
      compositedFramesCount_( 0 ) ,
      compositor_( nullptr )
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

}

int CLAbstractParallelRenderer::getCLRenderersCount() const
{
    return renderers_.size();
}



void CLAbstractParallelRenderer::startRendering()
{
    activeRenderers_ =  inUseGPUs_.size();

    LOG_INFO("Triggering Rendering Nodes");

    //First spark of the rendering loop.
    applyTransformation_();

    LOG_INFO("[DONE] Triggering Rendering Nodes");
}

void CLAbstractParallelRenderer::applyTransformation_()
{
    readyPixmapsCount_ = 0 ;

    TIC( frameworkProfile.renderingLoop_TIMER );

    // fetch new transformations if exists.
    syncTransformation_();

    for( const oclHWDL::Device *renderingDevice : inUseGPUs_ )
    {
        const Renderer::CLAbstractRenderer *renderer =
                renderers_[ renderingDevice ];

        TIC( renderingProfiles.value( renderer )->threadSpawning_TIMER );
        // Spawn threads and start rendering on each rendering node.
        rendererPool_.start( renderingTasks_[ renderer ]);
    }

    pendingTransformations_ = false;
    renderersReady_ = false;
}

void CLAbstractParallelRenderer::syncTransformation_()
{
    // modified when no active rendering threads
    transformationAsync_ = transformation_ ;
}


const Renderer::CLAbstractRenderer &
CLAbstractParallelRenderer::getCLRenderer( const uint64_t gpuIndex ) const
{
    // handle some minor exceptions.
    const oclHWDL::Device *device = listGPUs_.at( gpuIndex );

    if(!( inUseGPUs_.contains( device )))
        LOG_ERROR("No such rendering node!");

    return *renderers_[ device ];

}



const Compositor::CLAbstractCompositor
&CLAbstractParallelRenderer::getCLCompositor() const
{
    return  *compositor_  ;
}


uint CLAbstractParallelRenderer::getMachineGPUsCount() const
{
    return machineGPUsCount_;
}

uint64_t CLAbstractParallelRenderer::getFrameWidth() const
{
    return frameWidth_;
}

uint64_t CLAbstractParallelRenderer::getFrameHeight() const
{
    return frameHeight_;
}

void CLAbstractParallelRenderer::updateRotationX_SLOT( int angle )
{
    transformation_.rotation.x = angle ;
    if( renderersReady_ ) applyTransformation_();
    pendingTransformations_ = true ;
}

void CLAbstractParallelRenderer::updateRotationY_SLOT( int angle )
{
    transformation_.rotation.y = angle ;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void CLAbstractParallelRenderer::updateRotationZ_SLOT( int angle )
{
    transformation_.rotation.z = angle ;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void CLAbstractParallelRenderer::updateTranslationX_SLOT( int distance )
{
    transformation_.translation.x = distance;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void CLAbstractParallelRenderer::updateTranslationY_SLOT( int distance )
{
    transformation_.translation.y = distance;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void CLAbstractParallelRenderer::updateTranslationZ_SLOT( int distance )
{
    transformation_.translation.z = distance;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void CLAbstractParallelRenderer::updateScaleX_SLOT( int distance )
{
    transformation_.scale.x = distance;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void CLAbstractParallelRenderer::updateScaleY_SLOT( int distance )
{
    transformation_.scale.y = distance;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void CLAbstractParallelRenderer::updateScaleZ_SLOT( int distance )
{
    transformation_.scale.z = distance;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void CLAbstractParallelRenderer::updateScaleXYZ_SLOT( int distance )
{
    transformation_.scale.x = distance;
    transformation_.scale.y = distance;
    transformation_.scale.z = distance;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void CLAbstractParallelRenderer::updateImageBrightness_SLOT( float brightness )
{
    transformation_.brightness = brightness;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void CLAbstractParallelRenderer::updateVolumeDensity_SLOT( float density )
{
    transformation_.volumeDensity = density;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void CLAbstractParallelRenderer::updateIsoValue_SLOT(float isoValue)
{
    transformation_.isoValue = isoValue;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void CLAbstractParallelRenderer::activateRenderingKernel_SLOT(
        clKernel::RenderingMode type )
{
    for( Renderer::CLAbstractRenderer *renderer : renderers_.values())
        renderer->switchRenderingKernel( type );

    if( compositor_ != nullptr )
        compositor_->switchCompositingKernel( type );


    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void CLAbstractParallelRenderer::benchmark_( )
{

    for( Renderer::CLAbstractRenderer *renderer : renderers_ )
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


}
}
