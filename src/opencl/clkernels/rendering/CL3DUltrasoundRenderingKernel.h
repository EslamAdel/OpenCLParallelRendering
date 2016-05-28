#ifndef CL3DULTRASOUNDRENDERINGKERNEL_H
#define CL3DULTRASOUNDRENDERINGKERNEL_H

#include "CLRenderingKernel.h"

namespace  clparen  {
namespace CLKernel {


class CL3DUltrasoundRenderingKernel : public CLRenderingKernel
{

    enum KernelArgument
    {
        KERNEL_ARG_TransferFunctionData = CLRenderingKernel::KERNEL_ARG_DerivedKernelsOffset ,
        KERNEL_ARG_TransferFunctionSampler ,
        KERNEL_ARG_ApexAngle ,
        KERNEL_ARG_XScale ,
        KERNEL_ARG_YScale ,
        KERNEL_ARG_ZScale
    };

public:
    CL3DUltrasoundRenderingKernel( cl_context clContext = NULL,
                           const std::string kernelDirectory = ".",
                           const std::string kernelFile = "ultrasound.cl" ,
                           const std::string kernelName = "ultrasound" );

public:

    void setTransferFunctionData( cl_mem data );

    void setTransferFunctionSampler( cl_sampler sampler );

    void setApexAngle( float apexAngle );

    void setXScale( float scale );
    void setYScale( float scale );
    void setZScale( float scale );

    bool isFramePrecisionSupported(
            CLData::FRAME_CHANNEL_TYPE precision ) override;

};


}
}

#endif // CL3DULTRASOUNDRENDERINGKERNEL_H
