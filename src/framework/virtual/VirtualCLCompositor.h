#ifndef VIRTUALCLCompositor_H
#define VIRTUALCLCompositor_H

#include "CLCompositor.h"
#include "VirtualCLRenderer.h"

class VirtualCLCompositor : public CLCompositor
{
public:
    VirtualCLCompositor( const uint64_t gpuIndex ,
                            const uint frameWidth ,
                            const uint frameHeight );


//    void collectFrame( const uint8_t frameIndex ,
//                       cl_command_queue sourceCmdQueue ,
//                       CLFrame32 &sourceFrame ,
//                       const cl_bool block );
};

#endif // VIRTUALCLCompositor_H
