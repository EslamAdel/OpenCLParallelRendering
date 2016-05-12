#include "CLRenderingKernel.h"
#include <Logger.h>

clparen::clKernel::CLRenderingKernel::CLRenderingKernel(
        cl_context clContext,
        const std::string kernelDirectory,
        const std::string kernelFile  )
    : clContext_( clContext ) ,
      kernelDirectory_( kernelDirectory ) ,
      kernelFile_( kernelFile )
{
    buildKernel_( );
}

void clparen::clKernel::CLRenderingKernel::buildKernel_(
        const std::string extraBuildOptions )
{
    LOG_INFO("Building the kernel");
    // Create a kernel context, that should handle every thing that is
    // relevant to the kernel.
    kernelContext_ = new oclHWDL::KernelContext
            ( kernelDirectory_ , kernelFile_, clContext_, 0 , 0 );

    // Add the build options.
    // TODO: Check the support of the hardware to CL_DEVICE_IMAGE_SUPPORT
    std::string buildOptions;
    buildOptions += "-cl-fast-relaxed-math";
    buildOptions += " -DIMAGE_SUPPORT";
    buildOptions += extraBuildOptions;
    kernelContext_->buildProgram(buildOptions);
}

std::string clparen::clKernel::CLRenderingKernel::getKernelDirectory( ) const
{
    return kernelDirectory_;
}

std::string clparen::clKernel::CLRenderingKernel::getKernelFile() const
{
    return kernelFile_;
}

oclHWDL::KernelContext*
clparen::clKernel::CLRenderingKernel::getKernelContext( ) const
{
    return kernelContext_;
}

cl_kernel
clparen::clKernel::CLRenderingKernel::getKernelObject( ) const
{
    return kernelObject_;
}


void clparen::clKernel::CLRenderingKernel::setFrameBuffer( cl_mem frameBuffer )
{
    cl_int error = clSetKernelArg( kernelObject_, KERNEL_ARG_FrameBuffer,
                                   sizeof( cl_mem ),
                                   ( void* ) &frameBuffer );

    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }

}

void clparen::clKernel::CLRenderingKernel::setFrameWidth( uint width )
{
    cl_int error = clSetKernelArg( kernelObject_, KERNEL_ARG_FrameWidth ,
                                   sizeof( uint ),
                                   &width );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}

void clparen::clKernel::CLRenderingKernel::setFrameHeight( uint height )
{
    cl_int error = clSetKernelArg( kernelObject_, KERNEL_ARG_FrameHeight,
                                   sizeof( uint ),
                                   &height );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}

void clparen::clKernel::CLRenderingKernel::setVolumeData( cl_mem data )
{
    cl_int error = clSetKernelArg( kernelObject_, KERNEL_ARG_Volume ,
                                   sizeof( cl_mem ),
                                   ( void* ) &data );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}

void clparen::clKernel::CLRenderingKernel::setVolumeSampler( cl_sampler sampler )
{
    cl_int error = clSetKernelArg( kernelObject_, KERNEL_ARG_VolumeSampler ,
                                   sizeof( cl_sampler ),
                                   &sampler );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}


void clparen::clKernel::CLRenderingKernel::setInverseViewMatrix( cl_mem matrix )
{
    cl_int error = clSetKernelArg( kernelObject_, KERNEL_ARG_InverseMatrix ,
                                   sizeof( cl_mem ),
                                   ( void* ) &matrix );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}


void clparen::clKernel::CLRenderingKernel::releaseKernel()
{
    if( kernelContext_->getProgram( ))
        clReleaseProgram( kernelContext_->getProgram() );

    if( kernelObject_ )
        clReleaseKernel( kernelObject_ );

}

clparen::clKernel::CLRenderingKernel::~CLRenderingKernel( )
{
    releaseKernel( );
}
