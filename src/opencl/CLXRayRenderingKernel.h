#ifndef CLXRAYRENDERINGKERNEL_H
#define CLXRAYRENDERINGKERNEL_H

#include <CLRenderingKernel.h>

class CLXRayRenderingKernel : public CLRenderingKernel
{

private:
    enum XRayKernelArgument
    {
        KERNEL_ARG_Density = KERNEL_ARG_DerivedKernelsOffset  ,
        KERNEL_ARG_Brightness
    };

public:
    CLXRayRenderingKernel( cl_context clContext = NULL,
                           const std::string kernelDirectory = ".",
                           const std::string kernelFile = "xray.cl" ,
                           const std::string kernelName = "xray" );

public:

    RenderingMode getRenderingKernelType( ) const override ;


    /**
     * @brief setVolumeDensityFactor
     * @param density
     */
    void setVolumeDensityFactor( float density ) override;

    /**
     * @brief setImageBrightnessFactor
     * @param brightness
     */
    void setImageBrightnessFactor( float brightness ) override;



protected:

    /**
     * @brief retrieveKernelObject_
     */
    void retrieveKernelObject_( ) override ;

private:

    /**
     * @brief kernelName_
     */
    const std::string kernelName_;
};

#endif // CLXRAYRENDERINGKERNEL_H
