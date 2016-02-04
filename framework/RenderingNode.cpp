#include "RenderingNode.h"
#include <Logger.h>

RenderingNode::RenderingNode(const uint64_t gpuIndex ,
                             const uint frameWidth, const uint frameHeight,
                             Coordinates3D &globalTranslation,
                             Coordinates3D &globalRotation,
                             float &volumeDensity, float &brightness,
                             const Volume<uchar> *subVolume)
    :
      CLContext< uchar >( subVolume , gpuIndex , frameWidth , frameHeight ) ,
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
                       imageBrightness_ );

    emit this->finishedRendering( this );
}
