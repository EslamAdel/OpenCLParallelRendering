#include "CLRewindFrameKernel.h"
#include "Logger.h"

CLRewindFrameKernel::CLRewindFrameKernel(const cl_context clContext,
                                         const std::string kernelDirectory,
                                         const std::string kernelFile,
                                         const std::string kernelName)
    : clContext_( clContext ),
      kernelDirectory_( kernelDirectory ),
      kernelFile_( kernelFile ),
      kernelName_( kernelName )
{
    buildKernel_();
    retrieveKernelObject_();
}

void CLRewindFrameKernel::setFrame(const cl_mem frameImage2D )
{
    cl_int error = CL_SUCCESS;

    error = clSetKernelArg( kernelObject_, KernelArguments::Frame ,
                            sizeof( cl_mem ),
                            ( void* ) &frameImage2D );

    oclHWDL::Error::checkCLError( error );
}

void CLRewindFrameKernel::setFrameWidth(const uint width)
{
    cl_int error = CL_SUCCESS;

    error = clSetKernelArg( kernelObject_, KernelArguments::FrameWidth ,
                            sizeof( uint ),
                            &width );

    oclHWDL::Error::checkCLError( error );
}

void CLRewindFrameKernel::setFrameHeight(const uint height)
{
    cl_int error = CL_SUCCESS;

    error = clSetKernelArg( kernelObject_, KernelArguments::FrameHeight ,
                            sizeof( uint ),
                            &height );

    oclHWDL::Error::checkCLError( error );
}

void CLRewindFrameKernel::releaseKernel()
{
    if( kernelContext_->getProgram( ))
        clReleaseProgram( kernelContext_->getProgram() );
}

cl_kernel CLRewindFrameKernel::getKernelObject() const
{
    return kernelObject_;
}


void CLRewindFrameKernel::buildKernel_(const std::string extraBuildOptions)
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

void CLRewindFrameKernel::retrieveKernelObject_()
{
    kernelObject_ = kernelContext_->getKernelObject( kernelName_ );
}
