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


#ifdef BENCHMARKING
//Profiles Difinitions
DEFINE_PROFILES

#define AUTO_ROTATE

#endif

namespace clparen
{
namespace Parallel
{

CLAbstractParallelRenderer::CLAbstractParallelRenderer(
        const uint64_t frameWidth,
        const uint64_t frameHeight,
        const CLData::FRAME_CHANNEL_ORDER channelOrder ,
        QObject *parent )
    : QObject( parent ) ,
      frameWidth_( frameWidth ) ,
      frameHeight_( frameHeight ),
      renderersReady_( false ),
      compositedFramesCount_( 0 ) ,
      frameChannelOrder_( channelOrder )
{

    listGPUs_ = clHardware_.getListGPUs();

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

    transformation_.maxSteps = 500 ;
    transformation_.stepSize = 0.01 ;
    transformation_.apexAngle = 0.1f ;

    compositor_ = 0;


    connect( this , SIGNAL(compositingFinished_SIGNAL( )) ,
             this , SLOT(compositingFinished_SLOT( )));



    connect( this , SIGNAL( finishedRendering_SIGNAL( uint )),
             this , SLOT( finishedRendering_SLOT( uint )));

    connect( this , SIGNAL( frameLoadedToDevice_SIGNAL( uint )) ,
             this , SLOT( frameLoadedToDevice_SLOT( uint )));


}

void CLAbstractParallelRenderer::startRendering()
{
    activeRenderers_ =  renderers_.size();

    LOG_INFO("Triggering Rendering Nodes");

    //First spark of the rendering loop.
    applyTransformation_();

    LOG_INFO("[DONE] Triggering Rendering Nodes");
}

void CLAbstractParallelRenderer::syncTransformation_()
{
    // modified when no active rendering threads
    transformationAsync_ = transformation_ ;
}

uint64_t CLAbstractParallelRenderer::getFrameWidth() const
{
    return frameWidth_;
}

uint64_t CLAbstractParallelRenderer::getFrameHeight() const
{
    return frameHeight_;
}

Compositor::CLAbstractCompositor &CLAbstractParallelRenderer::getCLCompositor()
{
    return *compositor_;
}

Renderer::CLAbstractRenderer &CLAbstractParallelRenderer::getCLRenderer(
            uint gpuIndex )
{
    return *renderers_[ gpuIndex ];
}

void CLAbstractParallelRenderer::updateRotationX_SLOT( int angle )
{
    transformation_.rotation.x = angle ;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
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

void CLAbstractParallelRenderer::updateMaxSteps_SLOT( int value )
{
    transformation_.maxSteps = value ;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void CLAbstractParallelRenderer::updateStepSize_SLOT( float value )
{
    transformation_.stepSize = value ;
    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}

void CLAbstractParallelRenderer::updateApexAngle_SLOT( int value )
{
    transformation_.apexAngle = value ;
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
            CLKernel::RenderingMode type )
{
    for( Renderer::CLAbstractRenderer *renderer : renderers_ )
        renderer->switchRenderingKernel( type );

    if( compositor_ != 0 )
        compositor_->switchCompositingKernel( type );


    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}


void CLAbstractParallelRenderer::assertThread_()
{
    if( QThread::currentThread() != this->thread())
        LOG_ERROR("Current thread != object thread");
}


void CLAbstractParallelRenderer::updateTransferFunction_SLOT(
        float *transferFunction , uint length )
{
    for( Renderer::CLAbstractRenderer *renderer : renderers_ )
        renderer->updateTransferFunction( transferFunction , length );

    if( renderersReady_ ) applyTransformation_();
    else pendingTransformations_ = true ;
}


}
}
