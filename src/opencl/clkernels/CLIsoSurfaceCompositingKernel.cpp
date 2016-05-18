#include "CLIsoSurfaceCompositingKernel.h"


clparen::clKernel::CLIsoSurfaceCompositingKernel::
CLIsoSurfaceCompositingKernel( const cl_context clContext ,
                                           const std::string kernelName ,
                                           const std::string kernelFile ,
                                           const std::string kernelDirectory )
    : CLCompositingKernel( clContext , kernelName , kernelFile , kernelDirectory )
{

}
