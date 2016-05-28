#include "CLCompositingKernel.h"


namespace clparen {
namespace CLKernel {


CLCompositingKernel::CLCompositingKernel(
        const cl_context clContext ,
        const std::string kernelName ,
        const std::string kernelFile ,
        const std::string kernelDirectory )
    : clContext_( clContext ),
      kernelDirectory_( kernelDirectory ),
      kernelFile_( kernelFile ),
      kernelName_( kernelName )
{
    buildKernel_();
    retrieveKernelObject_();
}

void CLCompositingKernel::setFinalFrame(const cl_mem collageFrame)
{

    cl_int error = CL_SUCCESS;

    error = clSetKernelArg( kernelObject_, KernelArgument::KERNEL_ARG_FinalFrame,
                            sizeof( cl_mem ),
                            ( void* ) &collageFrame );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting due to OpenCL Error!");
    }
}


void CLCompositingKernel::setFrame( const cl_mem frameImage2D )
{

    cl_int error = CL_SUCCESS;

    error = clSetKernelArg( kernelObject_, KernelArgument::KERNEL_ARG_Frame ,
                            sizeof( cl_mem ),
                            ( void* ) &frameImage2D );

    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting due to OpenCL Error!");
    }
}

void CLCompositingKernel::setDepthIndex( const cl_mem depthIndex )
{
    cl_int error = CL_SUCCESS;

    error = clSetKernelArg( kernelObject_, KernelArgument::KERNEL_ARG_DepthIndex ,
                            sizeof( cl_mem ) ,
                            ( void* ) &depthIndex );

    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting due to OpenCL Error!");
    }
}


void CLCompositingKernel::releaseKernel()
{
    if( kernelContext_->getProgram( ))
        clReleaseProgram( kernelContext_->getProgram() );
}

cl_kernel CLCompositingKernel::getKernelObject() const
{
    return kernelObject_;
}


void CLCompositingKernel::buildKernel_(const std::string extraBuildOptions )
{

    LOG_INFO("Building the kernel");
    // Create a kernel context, that should handle every thing that is
    // relevant to the kernel.
    kernelContext_ = new oclHWDL::KernelContext
            ( kernelDirectory_, kernelFile_, clContext_, 0 , 0 );

    // Add the build options.
    // TODO: Check the support of the hardware to CL_DEVICE_IMAGE_SUPPORT
    std::string buildOptions;
    buildOptions += "-cl-fast-relaxed-math";
    buildOptions += " -DIMAGE_SUPPORT";
    buildOptions += extraBuildOptions;
    kernelContext_->buildProgram(buildOptions);
}

void CLCompositingKernel::retrieveKernelObject_( )
{
    kernelObject_ = kernelContext_->getKernelObject( kernelName_ );
}


}
}
