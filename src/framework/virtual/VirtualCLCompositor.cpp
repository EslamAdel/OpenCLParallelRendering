#include "VirtualCLCompositor.h"
#include "Logger.h"

template< class T >
VirtualCLCompositor< T >::VirtualCLCompositor( const uint64_t gpuIndex ,
                                               const uint frameWidth ,
                                               const uint frameHeight )
    : CLCompositor< T >( gpuIndex , frameWidth , frameHeight )
{

}

#include "VirtualCLCompositor.ipp"
