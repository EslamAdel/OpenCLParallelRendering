#ifndef CLMinIntensityProjectionRENDERINGKERNEL_H
#define CLMinIntensityProjectionRENDERINGKERNEL_H

#include <CLRenderingKernel.h>

class CLMinIntensityProjectionRenderingKernel : public CLRenderingKernel
{
private:
    enum XRayKernelArgument
    {
        KERNEL_ARG_Density = KERNEL_ARG_DerivedKernelsOffset  ,
        KERNEL_ARG_Brightness
    };

public:
    CLMinIntensityProjectionRenderingKernel(
            cl_context sharedContext = NULL,
            const std::string kernelDirectory = ".",
            const std::string kernelFile = "minIntensityProjection.cl" ,
            const std::string kernelName = "minIntensityProjection" );

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

#endif // CLMinIntensityProjectionRENDERINGKERNEL_H
