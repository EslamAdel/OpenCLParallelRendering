#ifndef CLXRAYRENDERINGKERNEL_H
#define CLXRAYRENDERINGKERNEL_H

#include "CLRenderingKernel.h"

namespace clparen {
namespace clKernel {

class CLXRayRenderingKernel : public CLRenderingKernel
{

private:
    enum XRayKernelArgument
    {
        KERNEL_ARG_Density = KERNEL_ARG_DerivedKernelsOffset  ,
        KERNEL_ARG_Brightness
    };

public:
    /**
     * @brief CLXRayRenderingKernel
     * @param clContext
     * @param kernelDirectory
     * @param kernelFile
     * @param kernelName
     */
    CLXRayRenderingKernel( cl_context clContext = NULL,
                           const std::string kernelDirectory = ".",
                           const std::string kernelFile = "xray.cl" ,
                           const std::string kernelName = "xray" );

public:

    /**
     * @brief getRenderingKernelType
     * @return
     */
    virtual RenderingMode getRenderingKernelType( ) const final ;


    /**
     * @brief setVolumeDensityFactor
     * @param density
     */
    virtual void setVolumeDensityFactor( float density ) final;

    /**
     * @brief setImageBrightnessFactor
     * @param brightness
     */
    virtual void setImageBrightnessFactor( float brightness ) final;



protected:

    /**
     * @brief retrieveKernelObject_
     */
    virtual void retrieveKernelObject_( ) final ;

private:

    /**
     * @brief kernelName_
     */
    const std::string kernelName_;
};


}
}


#endif // CLXRAYRENDERINGKERNEL_H
