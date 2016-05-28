#ifndef CLAverageIntensityProjectionCompositingKernel_H
#define CLAverageIntensityProjectionCompositingKernel_H


#include "CLCompositingKernel.h"


namespace clparen {
namespace CLKernel {

class CLAverageIntensityProjectionCompositingKernel : public CLCompositingKernel
{
public:

    /**
     * @brief CLAverageIntensityProjectionCompositingKernel
     * @param clContext
     * @param kernelName
     * @param kernelFile
     * @param kernelDirectory
     */
    CLAverageIntensityProjectionCompositingKernel(
            const cl_context clContext ,
            const std::string kernelName = "averageIntensityProjection_compositing" ,
            const std::string kernelFile = "averageIntensityProjection_compositing.cl" ,
            const std::string kernelDirectory = "." );


};



}
}

#endif // CLAverageIntensityProjectionCompositingKernel_H
