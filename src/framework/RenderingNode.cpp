#include "RenderingNode.h"
#include <Logger.h>

RenderingNode::RenderingNode(const uint64_t gpuIndex ,
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


void RenderingNode::applyTransformation( )
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

const Coordinates3D &RenderingNode::getCurrentCenter() const
{
    return currentCenter_ ;
}


void RenderingNode::loadVolume(const Volume<uchar> *volume)
{
    this->loadVolume_( volume );
    currentCenter_ = volume->getUnitCubeCenter();
}

void RenderingNode::setFrameIndex(const uint frameIndex)
{
    frameIndex_ = frameIndex;
}

uint RenderingNode::getFrameIndex() const
{
    return frameIndex_;
}
