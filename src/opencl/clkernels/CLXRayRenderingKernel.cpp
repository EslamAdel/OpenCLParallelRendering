#include "CLXRayRenderingKernel.h"
#include <oclUtils.h>
#include "Logger.h"

namespace clparen {
namespace CLKernel {


CLXRayRenderingKernel::CLXRayRenderingKernel(
        cl_context clContext,
        const std::string kernelDirectory,
        const std::string kernelFile,
        const std::string kernelName )
    : CLRenderingKernel( clContext, kernelDirectory, kernelFile ) ,
      kernelName_( kernelName )
{

    retrieveKernelObject_();
}

void CLXRayRenderingKernel::retrieveKernelObject_( )
{
    kernelObject_ = kernelContext_->getKernelObject( kernelName_ );
}

RenderingMode
CLXRayRenderingKernel::getRenderingKernelType( ) const
{
    return RenderingMode::RENDERING_MODE_Xray ;
}


void CLXRayRenderingKernel::
setImageBrightnessFactor( float brightness )
{
    cl_int error = clSetKernelArg( kernelObject_, KERNEL_ARG_Brightness ,
                                   sizeof( float ),
                                   &brightness );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}

void CLXRayRenderingKernel::
setVolumeDensityFactor( float density )
{
    cl_int error = clSetKernelArg( kernelObject_, KERNEL_ARG_Density ,
                                   sizeof( float ),
                                   &density );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}


}
}
