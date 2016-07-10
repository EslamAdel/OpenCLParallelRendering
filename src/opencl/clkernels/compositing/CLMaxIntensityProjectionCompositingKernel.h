#ifndef CLMaxIntensityProjectionCompositingKernel_H
#define CLMaxIntensityProjectionCompositingKernel_H


#include "CLCompositingKernel.h"


namespace clparen {
namespace CLKernel {

class CLMaxIntensityProjectionCompositingKernel : public CLCompositingKernel
{
public:

    /**
     * @brief CLMaxIntensityProjectionCompositingKernel
     * @param clContext
     * @param kernelName
     * @param kernelFile
     * @param kernelDirectory
     */
    CLMaxIntensityProjectionCompositingKernel(
            const cl_context clContext ,
            const std::string kernelName = "maxIntensityProjection_compositing_accumulate" ,
            const std::string kernelFile = "maxIntensityProjection_compositing.cl" ,
            const std::string kernelDirectory = "." );


};



}
}

#endif // CLMaxIntensityProjectionCompositingKernel_H
