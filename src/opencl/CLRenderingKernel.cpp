#include "CLRenderingKernel.h"
#include <Logger.h>

CLRenderingKernel::CLRenderingKernel( cl_context clContext,
                                      const std::string kernelDirectory,
                                      const std::string kernelFile )
    : clContext_( clContext )
    ,  kernelDirectory_( kernelDirectory )
    , kernelFile_( kernelFile )
{

}

void CLRenderingKernel::buildKernel_( const std::string extraBuildOptions )
{
    LOG_INFO("Building the kernel");
    // Create a kernel context, that should handle every thing that is
    // relevant to the kernel.
    kernelContext_ = new oclHWDL::KernelContext
            ( kernelDirectory_, kernelFile_, clContext_, NULL, NULL );

    // Add the build options.
    // TODO: Check the support of the hardware to CL_DEVICE_IMAGE_SUPPORT
    std::string buildOptions;
    buildOptions += "-cl-fast-relaxed-math";
    buildOptions += " -DIMAGE_SUPPORT";
    buildOptions += extraBuildOptions;
    kernelContext_->buildProgram(buildOptions);
}

std::string CLRenderingKernel::getKernelDirectory( ) const
{
    return kernelDirectory_;
}

std::string CLRenderingKernel::getKernelFile() const
{
    return kernelFile_;
}

oclHWDL::KernelContext* CLRenderingKernel::getKernelContext( ) const
{
    return kernelContext_;
}

cl_kernel CLRenderingKernel::getKernelObject( ) const
{
    return kernelObject_;
}

void CLRenderingKernel::setTransferFunctionData( cl_mem )
{
    LOG_WARNING( "Unimplemented setTransferFunctionData()" );
}

void CLRenderingKernel::setTransferFunctionSampler( cl_sampler )
{
    LOG_WARNING( "Unimplemented setTransferFunctionSampler()" );
}

void CLRenderingKernel::setTransferFunctionOffset( float )
{
    LOG_WARNING( "Unimplemented setTransferFunctionOffset()" );
}

void CLRenderingKernel::setTransferFunctionScale( float scale )
{
    LOG_WARNING( "Unimplemented setTransferFunctionScale()" );
}

void CLRenderingKernel::releaseKernel()
{
    if( kernelContext_->getProgram( ))
        clReleaseProgram( kernelContext_->getProgram() );
}

CLRenderingKernel::~CLRenderingKernel( )
{
    releaseKernel( );
}
