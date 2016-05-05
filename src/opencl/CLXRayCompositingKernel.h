#ifndef CLXRayCompositingKernel_H
#define CLXRayCompositingKernel_H

#include "CLCompositingKernel.h"

class CLXRayCompositingKernel : public CLCompositingKernel
{

public:
    CLXRayCompositingKernel(
            const cl_context clContext ,
            const std::string kernelName = "xray_compositing_accumulate" ,
            const std::string kernelFile = "xray_compositing.cl" ,
            const std::string kernelDirectory = "." );
};

#endif // CLXRayCompositingKernel_H
