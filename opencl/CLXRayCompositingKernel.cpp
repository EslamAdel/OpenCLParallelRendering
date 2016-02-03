#include "CLXRayCompositingKernel.h"
#include "Logger.h"


CLXRayCompositingKernel::CLXRayCompositingKernel(const cl_context clContext,
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

void CLXRayCompositingKernel::setCollegeFrame(const cl_mem collageImage2D)
{

    cl_int error = CL_SUCCESS;

    error = clSetKernelArg( kernelObject_, KernelArguments::CollageFrame,
                            sizeof( cl_mem ),
                            ( void* ) &collageImage2D );

    oclHWDL::Error::checkCLError( error );
}

void CLXRayCompositingKernel::setCollageWidth(const uint width)
{
    cl_int error = CL_SUCCESS;

    error = clSetKernelArg( kernelObject_, KernelArguments::CollageWidth ,
                            sizeof( uint ),
                            &width );

    oclHWDL::Error::checkCLError( error );
}

void CLXRayCompositingKernel::setCollageHeight(const uint height)
{
    cl_int error = CL_SUCCESS;

    error = clSetKernelArg( kernelObject_, KernelArguments::CollageHeight ,
                            sizeof( uint ),
                            &height );

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

void CLXRayCompositingKernel::setFrameWidth(const uint width)
{
    cl_int error = CL_SUCCESS;

    error = clSetKernelArg( kernelObject_, KernelArguments::FrameWidth ,
                            sizeof( uint ),
                            &width );

    oclHWDL::Error::checkCLError( error );
}

void CLXRayCompositingKernel::setFrameHeight(const uint height)
{
    cl_int error = CL_SUCCESS;

    error = clSetKernelArg( kernelObject_, KernelArguments::FrameHeight ,
                            sizeof( uint ),
                            &height );

    oclHWDL::Error::checkCLError( error );
}

void CLXRayCompositingKernel::setFrameCenterX(const float centerX)
{
    cl_int error = CL_SUCCESS;

    error = clSetKernelArg( kernelObject_, KernelArguments::FrameCenterX ,
                            sizeof( float ),
                            &centerX );

    oclHWDL::Error::checkCLError( error );
}

void CLXRayCompositingKernel::setFrameCenterY(const float centerY)
{
    cl_int error = CL_SUCCESS;

    error = clSetKernelArg( kernelObject_, KernelArguments::FrameCenterY ,
                            sizeof( float ),
                            &centerY );

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

