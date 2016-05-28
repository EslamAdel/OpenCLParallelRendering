#ifndef CLIsoSurfaceCompositingKernel_H
#define CLIsoSurfaceCompositingKernel_H


#include "CLCompositingKernel.h"


namespace clparen {
namespace CLKernel {

class CLIsoSurfaceCompositingKernel : public CLCompositingKernel
{
public:

    /**
     * @brief CLIsoSurfaceCompositingKernel
     * @param clContext
     * @param kernelName
     * @param kernelFile
     * @param kernelDirectory
     */
    CLIsoSurfaceCompositingKernel(
            const cl_context clContext ,
            const std::string kernelName = "isoSurface_compositing" ,
            const std::string kernelFile = "isoSurface_compositing.cl" ,
            const std::string kernelDirectory = "." );


};



}
}

#endif // CLISOSURFACECompositingKernel_H
