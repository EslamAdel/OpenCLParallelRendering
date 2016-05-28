#ifndef CLMinIntensityProjectionRENDERINGKERNEL_H
#define CLMinIntensityProjectionRENDERINGKERNEL_H

#include "CLXRayRenderingKernel.h"

namespace  clparen {
namespace CLKernel {


class CLMinIntensityProjectionRenderingKernel : public CLXRayRenderingKernel
{

public:
    /**
     * @brief CLMinIntensityProjectionRenderingKernel
     * @param sharedContext
     * @param kernelDirectory
     * @param kernelFile
     * @param kernelName
     */
    CLMinIntensityProjectionRenderingKernel(
            cl_context sharedContext = NULL,
            const std::string kernelDirectory = ".",
            const std::string kernelFile = "minIntensityProjection.cl" ,
            const std::string kernelName = "minIntensityProjection" );


};


}
}
#endif // CLMinIntensityProjectionRENDERINGKERNEL_H
