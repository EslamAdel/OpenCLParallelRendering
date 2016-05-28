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
    : CLXRayRenderingKernel( clContext , kernelDirectory ,
                             kernelFile , kernelName )
{

}

void CLIsoSurfaceRenderingKernel::setVolumeIsoValue( float isoValue )
{
    cl_int error = clSetKernelArg( kernelObject_,
                                   KERNEL_ARG_XRayDerivedKernelOffset ,
                                   sizeof( float ),
                                   ( void* ) &isoValue );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}



}
}
