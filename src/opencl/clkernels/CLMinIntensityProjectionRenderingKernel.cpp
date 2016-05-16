#include "CLMinIntensityProjectionRenderingKernel.h"
#include <oclUtils.h>
#include <Logger.h>


namespace clparen {
namespace clKernel {


CLMinIntensityProjectionRenderingKernel::
CLMinIntensityProjectionRenderingKernel( cl_context clContext,
                                         const std::string kernelDirectory,
                                         const std::string kernelFile,
                                         const std::string kernelName )
    : CLRenderingKernel( clContext, kernelDirectory, kernelFile ) ,
      kernelName_( kernelName )
{
    retrieveKernelObject_();
}

void CLMinIntensityProjectionRenderingKernel::
retrieveKernelObject_( )
{
    kernelObject_ = kernelContext_->getKernelObject( kernelName_ );
}

RenderingMode
CLMinIntensityProjectionRenderingKernel::
getRenderingKernelType( ) const
{
    return RenderingMode::RENDERING_MODE_MinIntensity;
}


void CLMinIntensityProjectionRenderingKernel::
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

void CLMinIntensityProjectionRenderingKernel::
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