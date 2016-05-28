#include "CLAverageIntensityProjectionRenderingKernel.h"
#include <oclUtils.h>
#include <Logger.h>

namespace  clparen  {
namespace CLKernel {


CLAverageIntensityProjectionRenderingKernel::
CLAverageIntensityProjectionRenderingKernel(
        cl_context clContext,
        const std::string kernelDirectory,
        const std::string kernelFile,
        const std::string kernelName )
    : CLXRayRenderingKernel( clContext , kernelDirectory ,
                             kernelFile , kernelName )
{

}


}
}
