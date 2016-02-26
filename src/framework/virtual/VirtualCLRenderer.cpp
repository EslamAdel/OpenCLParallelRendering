#include "VirtualCLRenderer.h"

VirtualCLRenderer::VirtualCLRenderer( const uint64_t gpuIndex,
                                      const uint frameWidth ,
                                      const uint frameHeight ,
                                      const Transformation &transformation )
    : CLRenderer( 0 , frameWidth , frameHeight , transformation  ) ,
      gpuIndex_VIRTUAL_( gpuIndex )
{

}

uint64_t VirtualCLRenderer::getGPUIndex() const
{
    return gpuIndex_VIRTUAL_ ;
}

