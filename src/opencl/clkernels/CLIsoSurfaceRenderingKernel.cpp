#include "CLIsoSurfaceRenderingKernel.h"
#include <oclUtils.h>
#include <Logger.h>

clparen::CLKernel::CLIsoSurfaceRenderingKernel::
CLIsoSurfaceRenderingKernel( cl_context clContext ,
                                         const std::string kernelDirectory ,
                                         const std::string kernelFile ,
                                         const std::string kernelName )
    : CLRenderingKernel( clContext, kernelDirectory, kernelFile ) ,
      kernelName_( kernelName )
{
    retrieveKernelObject_();
}

void
clparen::CLKernel::CLIsoSurfaceRenderingKernel::retrieveKernelObject_( )
{
    kernelObject_ = kernelContext_->getKernelObject( kernelName_ );
}

clparen::CLKernel::RenderingMode
clparen::CLKernel::CLIsoSurfaceRenderingKernel::
getRenderingKernelType( ) const
{
    return RenderingMode::RENDERING_MODE_MinIntensity;
}


void clparen::CLKernel::CLIsoSurfaceRenderingKernel::
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

void clparen::CLKernel::CLIsoSurfaceRenderingKernel::
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

void clparen::CLKernel::CLIsoSurfaceRenderingKernel::
setVolumeIsoValue( float isovalue )
{
    cl_int error = clSetKernelArg( kernelObject_, KERNEL_ARG_IsoValue ,
                                   sizeof( float ),
                                   &isovalue );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error! [Arg:%d]",
                  KERNEL_ARG_IsoValue );
    }

}
