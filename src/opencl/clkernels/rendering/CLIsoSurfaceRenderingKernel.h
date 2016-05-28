#ifndef CLIsoSurfaceRENDERINGKERNEL_H
#define CLIsoSurfaceRENDERINGKERNEL_H

#include "CLRenderingKernel.h"


namespace clparen  {
namespace CLKernel {


class CLIsoSurfaceRenderingKernel : public CLRenderingKernel
{

private:
    enum KernelArgument
    {
        KERNEL_ARG_IsoValue = CLRenderingKernel::KERNEL_ARG_DerivedKernelsOffset
    };

public:
    CLIsoSurfaceRenderingKernel(
            cl_context sharedContext = NULL,
            const std::string kernelDirectory = ".",
            const std::string kernelFile = "isoSurface.cl" ,
            const std::string kernelName = "isoSurface" );

    void setVolumeIsoValue( float isoValue ) override ;


};



}
}

#endif // CLISOSURFACERENDERINGKERNEL_H
