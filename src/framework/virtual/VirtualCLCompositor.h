#ifndef VIRTUALCLCompositor_H
#define VIRTUALCLCompositor_H

#include "CLCompositor.h"
#include "VirtualCLRenderer.h"

template< class T >
class VirtualCLCompositor : public CLCompositor< T >
{
public:
    VirtualCLCompositor( const uint64_t gpuIndex ,
                         const uint frameWidth ,
                         const uint frameHeight );
};

#endif // VIRTUALCLCompositor_H
