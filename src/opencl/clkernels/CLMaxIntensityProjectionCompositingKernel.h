#ifndef CLMaxIntensityProjectionCompositingKernel_H
#define CLMaxIntensityProjectionCompositingKernel_H


#include "CLCompositingKernel.h"


namespace clparen {
namespace clKernel {

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
            const std::string kernelName = "maxIntensityProjection_compositing" ,
            const std::string kernelFile = "maxIntensityProjection_compositing.cl" ,
            const std::string kernelDirectory = "." );


};



}
}

#endif // CLMaxIntensityProjectionCompositingKernel_H
