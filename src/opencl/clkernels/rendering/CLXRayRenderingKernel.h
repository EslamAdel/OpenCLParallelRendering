#ifndef CLXRAYRENDERINGKERNEL_H
#define CLXRAYRENDERINGKERNEL_H

#include "CLRenderingKernel.h"

namespace clparen {
namespace CLKernel {

class CLXRayRenderingKernel : public CLRenderingKernel
{

public:
    /**
     * @brief CLXRayRenderingKernel
     * @param clContext
     * @param kernelDirectory
     * @param kernelFile
     * @param kernelName
     */
    CLXRayRenderingKernel( cl_context clContext = 0 ,
                           const std::string kernelDirectory = ".",
                           const std::string kernelFile = "xray.cl" ,
                           const std::string kernelName = "xray" );

};


}
}


#endif // CLXRAYRENDERINGKERNEL_H
