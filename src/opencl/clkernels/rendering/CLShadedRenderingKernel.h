#ifndef CLSHADEDRENDERINGKERNEL_H
#define CLSHADEDRENDERINGKERNEL_H

#include "CLRenderingKernel.h"

namespace clparen {
namespace CLKernel {

class CLShadedRenderingKernel : public CLRenderingKernel
{
private:
    enum KernelArgument
    {
        KERNEL_ARG_TransferFunctionData = CLRenderingKernel::KERNEL_ARG_DerivedKernelsOffset ,
        KERNEL_ARG_TransferFunctionSampler
    };

public:
    CLShadedRenderingKernel( cl_context clContext = NULL,
                           const std::string kernelDirectory = ".",
                           const std::string kernelFile = "shaded.cl" ,
                           const std::string kernelName = "shaded" );

public:

    void setTransferFunctionData( cl_mem data ) Q_DECL_OVERRIDE ;

    void setTransferFunctionSampler( cl_sampler sampler ) Q_DECL_OVERRIDE ;


};

}
}
#endif // CLSHADEDRENDERINGKERNEL_H
