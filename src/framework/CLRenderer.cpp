#include "CLRenderer.h"
#include <Logger.h>

CLRenderer::CLRenderer(const uint64_t gpuIndex ,
                             const uint frameWidth, const uint frameHeight,
                             const Coordinates3D &globalTranslation,
                             const Coordinates3D &globalRotation,
                             const Coordinates3D &globalScale,
                             const float &volumeDensity,
                             const float &brightness,
                             const float &transferScale,
                             const float &transferOffset)
    : CLContext< uchar >( gpuIndex , frameWidth , frameHeight , nullptr ) ,
      translation_( globalTranslation ),
      rotation_( globalRotation ),
      scale_( globalScale ),
      volumeDensity_( volumeDensity ),
      imageBrightness_( brightness ),
      transferFunctionScale_(transferScale),
      transferFunctionOffset_(transferOffset)
{
    LOG_INFO( "Creating Context on Node with GPU <%d>", gpuIndex );


}


void CLRenderer::applyTransformation( )
{

    this->paint( rotation_,
                 translation_,
                 scale_,
                 volumeDensity_,
                 imageBrightness_ ,
                 transferFunctionScale_,
                 transferFunctionOffset_,
                 currentCenter_
                 );

    emit this->finishedRendering( this );
}

const Coordinates3D &CLRenderer::getCurrentCenter() const
{
    return currentCenter_ ;
}


void CLRenderer::loadVolume(const Volume<uchar> *volume)
{
    this->loadVolume_( volume );
    currentCenter_ = volume->getUnitCubeCenter();
}

void CLRenderer::setFrameIndex(const uint frameIndex)
{
    frameIndex_ = frameIndex;
}

uint CLRenderer::getFrameIndex() const
{
    return frameIndex_;
}

void CLRenderer::setTransferFunctionFlag(int flag)
{
    activeRenderingKernel_->setTransferFunctionFlag(flag);
}
