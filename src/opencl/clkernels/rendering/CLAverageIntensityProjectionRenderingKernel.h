#ifndef CLAverageIntensityProjectionRENDERINGKERNEL_H
#define CLAverageIntensityProjectionRENDERINGKERNEL_H

#include "CLRenderingKernel.h"


namespace  clparen  {
namespace CLKernel {


class CLAverageIntensityProjectionRenderingKernel : public CLRenderingKernel
{

public:
    CLAverageIntensityProjectionRenderingKernel(
            cl_context sharedContext = NULL,
            const std::string kernelDirectory = ".",
            const std::string kernelFile = "averageIntensityProjection.cl" ,
            const std::string kernelName = "averageIntensityProjection" );


    bool isFramePrecisionSupported(
            CLData::FRAME_CHANNEL_TYPE precision ) override;
};



}
}

#endif // CLAverageIntensityProjectionRENDERINGKERNEL_H
