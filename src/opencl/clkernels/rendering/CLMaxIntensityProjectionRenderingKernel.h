#ifndef CLMaxIntensityProjectionRENDERINGKERNEL_H
#define CLMaxIntensityProjectionRENDERINGKERNEL_H

#include "CLRenderingKernel.h"


namespace clparen  {
namespace CLKernel {


class CLMaxIntensityProjectionRenderingKernel : public CLRenderingKernel
{


public:
    CLMaxIntensityProjectionRenderingKernel(
            cl_context context = NULL,
            const std::string kernelDirectory = ".",
            const std::string kernelFile = "maxIntensityProjection.cl" ,
            const std::string kernelName = "maxIntensityProjection" );


    bool isFramePrecisionSupported(
            CLData::FRAME_CHANNEL_TYPE precision ) override;
};



}
}

#endif // CLMaxIntensityProjectionRENDERINGKERNEL_H
