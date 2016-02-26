#include "CLXRayCompositingKernel.h"
#include "Logger.h"


CLXRayCompositingKernel::CLXRayCompositingKernel( const cl_context clContext ,
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

void CLXRayCompositingKernel::setCollageFrame(const cl_mem collageFrame)
{

    cl_int error = CL_SUCCESS;

    error = clSetKernelArg( kernelObject_, KernelArguments::CollageFrame,
                            sizeof( cl_mem ),
                            ( void* ) &collageFrame );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting due to OpenCL Error!");
    }
}


void CLXRayCompositingKernel::setFrame( const cl_mem frameImage2D )
{

    cl_int error = CL_SUCCESS;

    error = clSetKernelArg( kernelObject_, KernelArguments::Frame ,
                            sizeof( cl_mem ),
                            ( void* ) &frameImage2D );

    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting due to OpenCL Error!");
    }
}

void CLXRayCompositingKernel::setDepthIndex( const cl_mem depthIndex )
{
    cl_int error = CL_SUCCESS;

//    LOG_DEBUG( "setting depth index");
//    for( uint val : depth )
//        std::cout << val << std::endl ;

//    LOG_DEBUG("buffer size in bytes = %d" , sizeof(uint)*depth.size());
//    const uint *data = depth.constData();

    error = clSetKernelArg( kernelObject_, KernelArguments::DepthIndex ,
                            sizeof( cl_mem ) ,
                            ( void* ) &depthIndex );

    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting due to OpenCL Error!");
    }
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


void CLXRayCompositingKernel::buildKernel_(const std::string extraBuildOptions )
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

