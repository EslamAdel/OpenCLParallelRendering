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

#endif // CLMinIntensityProjectionRENDERINGKERNEL_H
