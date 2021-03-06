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



}
}
