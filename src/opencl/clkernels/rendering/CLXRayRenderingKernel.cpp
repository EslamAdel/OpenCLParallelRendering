#include "CLXRayRenderingKernel.h"
#include <oclUtils.h>
#include "Logger.h"

namespace clparen {
namespace CLKernel {


CLXRayRenderingKernel::CLXRayRenderingKernel(
        cl_context clContext,
        const std::string kernelDirectory,
        const std::string kernelFile,
        const std::string kernelName )
    : CLRenderingKernel( clContext, CLData::FRAME_CHANNEL_ORDER::ORDER_INTENSITY,
                         kernelName , kernelDirectory, kernelFile )
{
    buildKernel_();
    retrieveKernelObject_();
}

void CLXRayRenderingKernel::setMaxSteps( uint maxSteps )
{
    cl_int error = clSetKernelArg( kernelObject_,
                                   KERNEL_ARG_MaxSteps ,
                                   sizeof( uint ),
                                   ( void* ) &maxSteps );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}

void CLXRayRenderingKernel::setStepSize( float tStep )
{
    cl_int error = clSetKernelArg( kernelObject_,
                                   KERNEL_ARG_StepSize ,
                                   sizeof( float ),
                                   ( void* ) &tStep );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}


}
}
