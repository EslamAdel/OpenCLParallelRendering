#include "CLXRayCompositingKernel.h"

namespace clparen {
namespace CLKernel {


CLXRayCompositingKernel::CLXRayCompositingKernel(
        const cl_context clContext ,
        const std::string kernelName ,
        const std::string kernelFile ,
        const std::string kernelDirectory )
    : CLCompositingKernel( clContext , CLData::FRAME_CHANNEL_ORDER::ORDER_INTENSITY ,
                           kernelName , kernelFile , kernelDirectory )
{

}


}
}
