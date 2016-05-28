#ifndef CLXRAYRENDERINGKERNEL_H
#define CLXRAYRENDERINGKERNEL_H

#include "CLRenderingKernel.h"

namespace clparen {
namespace CLKernel {

class CLXRayRenderingKernel : public CLRenderingKernel
{
protected:
    enum XRayKernelArgument
    {
        KERNEL_ARG_MaxSteps = CLRenderingKernel::KERNEL_ARG_DerivedKernelsOffset ,
        KERNEL_ARG_StepSize ,
        KERNEL_ARG_XRayDerivedKernelOffset
    };

public:
    /**
     * @brief CLXRayRenderingKernel
     * @param clContext
     * @param kernelDirectory
     * @param kernelFile
     * @param kernelName
     */
    CLXRayRenderingKernel( cl_context clContext = 0 ,
                           const std::string kernelDirectory = ".",
                           const std::string kernelFile = "xray.cl" ,
                           const std::string kernelName = "xray" );



    void setMaxSteps( uint maxSteps ) Q_DECL_OVERRIDE ;

    void setStepSize( float tStep ) Q_DECL_OVERRIDE ;

};


}
}


#endif // CLXRAYRENDERINGKERNEL_H
