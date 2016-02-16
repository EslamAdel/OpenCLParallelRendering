#include "CLXRayCompositingKernel.h"
#include "Logger.h"


CLXRayCompositingKernel::CLXRayCompositingKernel( const cl_context clContext,
                                                  const std::string kernelName,
                                                  const std::string kernelFile,
                                                  const std::string kernelDirectory )
    : clContext_( clContext ),
      kernelDirectory_( kernelDirectory ),
      kernelFile_( kernelFile ),
      kernelName_( kernelName )
{
    buildKernel_();
    retrieveKernelObject_();
}

void CLXRayCompositingKernel::setCollageFrame(const cl_mem collageFrame)
{

    cl_int error = CL_SUCCESS;

    error = clSetKernelArg( kernelObject_, KernelArguments::CollageFrame,
                            sizeof( cl_mem ),
                            ( void* ) &collageFrame );

    oclHWDL::Error::checkCLError( error );
}


void CLXRayCompositingKernel::setFrame(const cl_mem frameImage2D)
{

    cl_int error = CL_SUCCESS;

    error = clSetKernelArg( kernelObject_, KernelArguments::Frame ,
                            sizeof( cl_mem ),
                            ( void* ) &frameImage2D );

    oclHWDL::Error::checkCLError( error );
}


void CLXRayCompositingKernel::releaseKernel()
{
    if( kernelContext_->getProgram( ))
        clReleaseProgram( kernelContext_->getProgram() );
}

cl_kernel CLXRayCompositingKernel::getKernelObject() const
{
    return kernelObject_;
}


void CLXRayCompositingKernel::buildKernel_(const std::string extraBuildOptions)
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

void CLXRayCompositingKernel::retrieveKernelObject_( )
{
    kernelObject_ = kernelContext_->getKernelObject( kernelName_ );
}
