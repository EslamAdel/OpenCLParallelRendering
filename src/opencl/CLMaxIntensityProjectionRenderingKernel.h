#ifndef CLMaxIntensityProjectionRENDERINGKERNEL_H
#define CLMaxIntensityProjectionRENDERINGKERNEL_H

#include <CLRenderingKernel.h>


class CLMaxIntensityProjectionRenderingKernel : public CLRenderingKernel
{
private:
    enum XRayKernelArgument
    {
        KERNEL_ARG_Density = KERNEL_ARG_DerivedKernelsOffset  ,
        KERNEL_ARG_Brightness
    };

public:
    CLMaxIntensityProjectionRenderingKernel(
            cl_context sharedContext = NULL,
            const std::string kernelDirectory = ".",
            const std::string kernelFile = "maxIntensityProjection.cl" ,
            const std::string kernelName = "maxIntensityProjection" );

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


#endif // CLMaxIntensityProjectionRENDERINGKERNEL_H
