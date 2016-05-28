#include "CL3DUltrasoundRenderingKernel.h"
#include <oclUtils.h>
#include <Logger.h>



/*
 * __kernel void
ultrasound(__global uint *d_output,
         uint imageW, uint imageH,
         float density, float brightness,
         __constant float* invViewMatrix
         ,__read_only image3d_t volume,
         __read_only image2d_t transferFunc,
         sampler_t volumeSampler,
         sampler_t transferFuncSampler, float apex
         )
*/
namespace  clparen  {
namespace CLKernel {

CL3DUltrasoundRenderingKernel::CL3DUltrasoundRenderingKernel(
        cl_context clContext,
        const std::string kernelDirectory,
        const std::string kernelFile,
        const std::string kernelName )
    : CLRenderingKernel( clContext, kernelName , kernelDirectory, kernelFile )
{
    buildKernel_( );
    retrieveKernelObject_();
}

void CL3DUltrasoundRenderingKernel::setTransferFunctionData( cl_mem data )
{
    cl_int error = clSetKernelArg( kernelObject_,
                                   KERNEL_ARG_TransferFunctionData,
                                   sizeof( cl_mem ),
                                   ( void* ) &data );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}

void CL3DUltrasoundRenderingKernel::setTransferFunctionSampler( cl_sampler sampler )
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

void CL3DUltrasoundRenderingKernel::setApexAngle( float apexAngle )
{
    cl_int error = clSetKernelArg( kernelObject_, KERNEL_ARG_ApexAngle,
                                   sizeof( float ),
                                   &apexAngle );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
    printf("apex %f \n", apexAngle );
}

void CL3DUltrasoundRenderingKernel::setXScale( float scale )
{
    cl_int error = clSetKernelArg( kernelObject_, KERNEL_ARG_XScale,
                                   sizeof( float ),
                                   &scale );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}

void CL3DUltrasoundRenderingKernel::setYScale( float scale )
{
    cl_int error = clSetKernelArg( kernelObject_, KERNEL_ARG_YScale,
                                   sizeof( float ),
                                   &scale );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}
void CL3DUltrasoundRenderingKernel::setZScale( float scale )
{
    cl_int error = clSetKernelArg( kernelObject_, KERNEL_ARG_ZScale,
                                   sizeof( float ),
                                   &scale );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}

bool CL3DUltrasoundRenderingKernel::isFramePrecisionSupported(
        CLData::FRAME_CHANNEL_TYPE precision )
{
    return framePrecision_RGBA_.contains( precision );
}

}
}
