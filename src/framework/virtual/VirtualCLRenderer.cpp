#include "VirtualCLRenderer.h"

VirtualCLRenderer::VirtualCLRenderer( const uint64_t gpuIndex,
                                      const uint frameWidth ,
                                      const uint frameHeight ,
                                      const Coordinates3D &globalTranslation,
                                      const Coordinates3D &globalRotation,
                                      const Coordinates3D &globalScale,
                                      const float &volumeDensity,
                                      const float &brightness,
                                      const float &transferScale,
                                      const float &transferOffset)
    : CLRenderer( 0 , frameWidth , frameHeight , globalTranslation ,
                  globalRotation , globalScale ,volumeDensity , brightness,
                  transferScale,transferOffset  ) ,
      gpuIndex_VIRTUAL_( gpuIndex )
{

}

uint64_t VirtualCLRenderer::getGPUIndex() const
{
    return gpuIndex_VIRTUAL_ ;
}

