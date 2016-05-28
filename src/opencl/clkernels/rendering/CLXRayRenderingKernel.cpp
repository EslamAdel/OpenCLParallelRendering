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
    : CLRenderingKernel( clContext, kernelName , kernelDirectory, kernelFile )
{
    buildKernel_();
    retrieveKernelObject_();
}

bool CLXRayRenderingKernel::isFramePrecisionSupported(
        CLData::FRAME_CHANNEL_TYPE precision )
{
    return framePrecision_Luminance_.contains( precision );
}


}
}
