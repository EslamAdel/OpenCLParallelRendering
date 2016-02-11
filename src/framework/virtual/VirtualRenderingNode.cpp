#include "VirtualRenderingNode.h"

VirtualRenderingNode::VirtualRenderingNode( const uint64_t gpuIndex,
                                            const uint frameWidth ,
                                            const uint frameHeight ,
                                            const Coordinates3D &globalTranslation,
                                            const Coordinates3D &globalRotation,
                                            const float &volumeDensity,
                                            const float &brightness )
    : RenderingNode( 0 , frameWidth , frameHeight , globalTranslation ,
                     globalRotation , volumeDensity , brightness  ) ,
      gpuIndex_VIRTUAL_( gpuIndex )
{

}

uint64_t VirtualRenderingNode::getGPUIndex() const
{
    return gpuIndex_VIRTUAL_ ;
}

