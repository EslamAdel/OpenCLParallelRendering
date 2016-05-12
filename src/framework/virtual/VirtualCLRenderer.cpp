#include "VirtualCLRenderer.h"


template< class V , class F >
clparen::Renderer::VirtualCLRenderer< V , F >::VirtualCLRenderer(
        const uint64_t gpuIndex,
        const uint frameWidth ,
        const uint frameHeight ,
        const Transformation &transformation )
    : CLRenderer< V , F >( 0 , frameWidth , frameHeight , transformation  ) ,
      gpuIndex_VIRTUAL_( gpuIndex )
{

}

template< class V , class F >
uint64_t clparen::Renderer::VirtualCLRenderer< V , F >::getGPUIndex() const
{
    return gpuIndex_VIRTUAL_ ;
}

#include "VirtualCLRenderer.ipp"
