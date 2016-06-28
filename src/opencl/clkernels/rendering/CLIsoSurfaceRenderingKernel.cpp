#include "CLIsoSurfaceRenderingKernel.h"
#include <oclUtils.h>
#include <Logger.h>

namespace clparen  {
namespace CLKernel {


CLIsoSurfaceRenderingKernel::
CLIsoSurfaceRenderingKernel(
        cl_context clContext,
        const std::string kernelDirectory,
        const std::string kernelFile,
        const std::string kernelName )
    : CLRenderingKernel( clContext, CLData::FRAME_CHANNEL_ORDER::ORDER_INTENSITY,
                         kernelName , kernelDirectory, kernelFile )
{
    buildKernel_();
    retrieveKernelObject_();
}

void CLIsoSurfaceRenderingKernel::setVolumeIsoValue( float isoValue )
{
    cl_int error = clSetKernelArg( kernelObject_,
                                   KERNEL_ARG_DerivedKernelsOffset ,
                                   sizeof( float ),
                                   ( void* ) &isoValue );
    CL_ASSERT( error );
}



}
}
