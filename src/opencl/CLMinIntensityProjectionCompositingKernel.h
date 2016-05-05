#ifndef CLMinIntensityProjectionCompositingKernel_H
#define CLMinIntensityProjectionCompositingKernel_H


#include "CLCompositingKernel.h"

class CLMinIntensityProjectionCompositingKernel : public CLCompositingKernel
{
public:

    CLMinIntensityProjectionCompositingKernel(
            const cl_context clContext ,
            const std::string kernelName = "minIntensityProjection_compositing" ,
            const std::string kernelFile = "minIntensityProjection_compositing.cl" ,
            const std::string kernelDirectory = "." );


};

#endif // CLMinIntensityProjectionCompositingKernel_H
