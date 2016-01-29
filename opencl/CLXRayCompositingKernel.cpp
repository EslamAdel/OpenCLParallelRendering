#include "CLXRayCompositingKernel.h"
#include "Logger.h"



CLXRayCompositingKernel::CLXRayCompositingKernel(
        const uint frameWidth,
        const uint frameHeight,
        const uint elementFramesCount,
        const CLXRayCompositingKernel::Mode compositingMode,
        const cl_context clContext,
        const std::string kernelDirectory,
        const std::string kernelFile,
        const std::string kernelName)
    : frameWidth_( frameWidth ),
      frameHeight_( frameHeight ),
      framesCount_( elementFramesCount ),
      compositingMode_( compositingMode ),
      clContext_( clContext ),
      kernelDirectory_( kernelDirectory ),
      kernelFile_( kernelFile ),
      kernelName_( kernelName )
{
    buildKernel_();
    retrieveKernelObject_();
}



void CLXRayCompositingKernel::accumulateBuffer(const cl_mem baseBuffer,
                                               const cl_mem buffer ,
                                               cl_command_queue cmdQueue )
{
    clSetKernelArg( kernelObject_ ,
                    AccumulatorModeArgs::BaseBuffer ,
                    sizeof(cl_mem),
                    &baseBuffer );

    clSetKernelArg( kernelObject_ ,
                    AccumulatorModeArgs::StackedBuffer ,
                    sizeof(cl_mem),
                    &buffer );


    static const size_t globalWorkSize[2] = { frameWidth_ , frameHeight_ } ;

    clEnqueueNDRangeKernel( cmdQueue , kernelObject_ , 2 , NULL,
                            globalWorkSize, NULL, 0, NULL, NULL);
    clFinish( cmdQueue );
}

void CLXRayCompositingKernel::releaseKernel()
{
    if( kernelContext_->getProgram( ))
        clReleaseProgram( kernelContext_->getProgram() );
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

