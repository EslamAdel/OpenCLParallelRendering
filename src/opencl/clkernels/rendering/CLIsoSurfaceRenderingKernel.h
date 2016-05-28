#ifndef CLIsoSurfaceRENDERINGKERNEL_H
#define CLIsoSurfaceRENDERINGKERNEL_H

#include "CLXRayRenderingKernel.h"


namespace clparen  {
namespace CLKernel {


class CLIsoSurfaceRenderingKernel : public CLXRayRenderingKernel
{

private:
    enum KernelArgument
    {
        KERNEL_ARG_IsoValue =
        CLXRayRenderingKernel::KERNEL_ARG_XRayDerivedKernelOffset
    };

public:
    CLIsoSurfaceRenderingKernel(
            cl_context sharedContext = NULL,
            const std::string kernelDirectory = ".",
            const std::string kernelFile = "isoSurface.cl" ,
            const std::string kernelName = "isoSurface" );

    void setVolumeIsoValue( float isoValue ) Q_DECL_OVERRIDE ;


};



}
}

#endif // CLISOSURFACERENDERINGKERNEL_H
