#ifndef CLAverageIntensityProjectionRENDERINGKERNEL_H
#define CLAverageIntensityProjectionRENDERINGKERNEL_H

#include "CLXRayRenderingKernel.h"


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

};



}
}

#endif // CLAverageIntensityProjectionRENDERINGKERNEL_H
