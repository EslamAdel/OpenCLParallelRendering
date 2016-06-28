#include "CL3DUltrasoundRenderingKernel.h"
#include <oclUtils.h>
#include <Logger.h>


namespace  clparen  {
namespace CLKernel {

CL3DUltrasoundRenderingKernel::CL3DUltrasoundRenderingKernel(
        cl_context clContext,
        const std::string kernelDirectory,
        const std::string kernelFile,
        const std::string kernelName )
    : CLRenderingKernel( clContext, CLData::FRAME_CHANNEL_ORDER::ORDER_RGBA,
                         kernelName , kernelDirectory, kernelFile )
{
    kernelType_ = RenderingMode::RENDERING_MODE_Ultrasound ;
    buildKernel_( );
    retrieveKernelObject_( );
}

void CL3DUltrasoundRenderingKernel::setTransferFunctionData( cl_mem data )
{
    cl_int error = clSetKernelArg( kernelObject_,
                                   KERNEL_ARG_TransferFunctionData,
                                   sizeof( cl_mem ),
                                   ( void* ) &data );
    CL_ASSERT( error );
}

void CL3DUltrasoundRenderingKernel::setTransferFunctionSampler( cl_sampler sampler )
{
    cl_int error = clSetKernelArg( kernelObject_,
                                   KERNEL_ARG_TransferFunctionSampler,
                                   sizeof( cl_sampler ),
                                   ( void* ) &sampler );
    CL_ASSERT( error );
}

void CL3DUltrasoundRenderingKernel::setApexAngle( float apexAngle )
{
    cl_int error = clSetKernelArg( kernelObject_, KERNEL_ARG_ApexAngle,
                                   sizeof( float ),
                                   &apexAngle );
    CL_ASSERT( error );
}

void CL3DUltrasoundRenderingKernel::setXScale( float scale )
{
    cl_int error = clSetKernelArg( kernelObject_, KERNEL_ARG_XScale,
                                   sizeof( float ),
                                   &scale );
    CL_ASSERT( error );
}

void CL3DUltrasoundRenderingKernel::setYScale( float scale )
{
    cl_int error = clSetKernelArg( kernelObject_, KERNEL_ARG_YScale,
                                   sizeof( float ),
                                   &scale );
    CL_ASSERT( error );
}
void CL3DUltrasoundRenderingKernel::setZScale( float scale )
{
    cl_int error = clSetKernelArg( kernelObject_, KERNEL_ARG_ZScale,
                                   sizeof( float ),
                                   &scale );
    CL_ASSERT( error );
}


bool CL3DUltrasoundRenderingKernel::supportTransferFunction() const
{
    return true;
}


}
}
