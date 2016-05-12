#ifndef CLMinIntensityProjectionCompositingKernel_H
#define CLMinIntensityProjectionCompositingKernel_H


#include "CLCompositingKernel.h"

namespace clpar {
namespace clKernel {


class CLMinIntensityProjectionCompositingKernel : public CLCompositingKernel
{
public:

    /**
     * @brief CLMinIntensityProjectionCompositingKernel
     * @param clContext
     * @param kernelName
     * @param kernelFile
     * @param kernelDirectory
     */
    CLMinIntensityProjectionCompositingKernel(
            const cl_context clContext ,
            const std::string kernelName = "minIntensityProjection_compositing" ,
            const std::string kernelFile = "minIntensityProjection_compositing.cl" ,
            const std::string kernelDirectory = "." );


};


}
}



#endif // CLMinIntensityProjectionCompositingKernel_H
