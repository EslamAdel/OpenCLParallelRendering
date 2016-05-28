#include "CLShadedRenderingKernel.h"
#include <oclUtils.h>
#include <Logger.h>

#define FRAME_BUFFER_ARG                                              0
#define FRAME_WIDTH_ARG                                               1
#define FRAME_HEIGHT_ARG                                              2
#define VOLUME_DENSITY_ARG                                            3
#define IMAGE_BRIGHTNESS_ARG                                          4
#define INVERSE_MATRIX_ARG                                            5
#define VOLUME_ARRAY_ARG                                              6
#define TRANSFER_FUNCTION_ARG                                         7
#define VOLUME_SAMPLER_ARG                                            8
#define TRANSFER_FUNCTION_SAMPLER_ARG                                 9

/*
 * __kernel void
shaded(__global uint *d_output,
         uint imageW, uint imageH,
         float density, float brightness,
         __constant float* invViewMatrix
 #ifdef IMAGE_SUPPORT
          ,__read_only image3d_t volume,
          __read_only image2d_t transferFunc,
          sampler_t volumeSampler,
          sampler_t transferFuncSampler
 #endif
         )
*/

namespace clparen {
namespace CLKernel {


CLShadedRenderingKernel::CLShadedRenderingKernel(
        cl_context clContext,
        const std::string kernelDirectory,
        const std::string kernelFile,
        const std::string kernelName )
    : CLRenderingKernel( clContext, CLData::FRAME_CHANNEL_ORDER::ORDER_RGBA,
                         kernelName , kernelDirectory, kernelFile )
{
    buildKernel_( );
    retrieveKernelObject_();
}


void CLShadedRenderingKernel::setTransferFunctionData( cl_mem data )
{
    cl_int error = clSetKernelArg( kernelObject_,
                                   KERNEL_ARG_TransferFunctionData ,
                                   sizeof( cl_mem ),
                                   ( void* ) &data );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}

void CLShadedRenderingKernel::setTransferFunctionSampler( cl_sampler sampler )
{
    cl_int error = clSetKernelArg( kernelObject_,
                                   KERNEL_ARG_TransferFunctionSampler,
                                   sizeof( cl_sampler ),
                                   ( void* ) &sampler );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}


}
}
