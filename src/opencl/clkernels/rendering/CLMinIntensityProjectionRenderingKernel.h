#ifndef CLMinIntensityProjectionRENDERINGKERNEL_H
#define CLMinIntensityProjectionRENDERINGKERNEL_H

#include "CLRenderingKernel.h"

namespace  clparen {
namespace CLKernel {


class CLMinIntensityProjectionRenderingKernel : public CLRenderingKernel
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

    bool isFramePrecisionSupported(
            CLData::FRAME_CHANNEL_TYPE precision ) override;
};


}
}
#endif // CLMinIntensityProjectionRENDERINGKERNEL_H
