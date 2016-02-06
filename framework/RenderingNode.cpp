#include "RenderingNode.h"
#include <Logger.h>

RenderingNode::RenderingNode(const uint64_t gpuIndex ,
                             const uint frameWidth, const uint frameHeight,
                             const Coordinates3D &globalTranslation,
                             const Coordinates3D &globalRotation,
                             const float &volumeDensity,
                             const float &brightness )
    : CLContext< uchar >( gpuIndex , frameWidth , frameHeight , nullptr ) ,
      translation_( globalTranslation ),
      rotation_( globalRotation ),
      volumeDensity_( volumeDensity ),
      imageBrightness_( brightness )
{
    LOG_INFO( "Creating Context on Node with GPU <%d>", gpuIndex );


}


void RenderingNode::applyTransformation( )
{

    this->paint( rotation_,
                 translation_,
                 volumeDensity_,
                 imageBrightness_ ,
                 currentCenter_ );

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
