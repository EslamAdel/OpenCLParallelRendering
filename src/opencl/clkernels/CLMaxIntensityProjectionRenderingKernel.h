#ifndef CLMaxIntensityProjectionRENDERINGKERNEL_H
#define CLMaxIntensityProjectionRENDERINGKERNEL_H

#include <CLRenderingKernel.h>


namespace  clparen  {
namespace clKernel {


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

#endif // CLMaxIntensityProjectionRENDERINGKERNEL_H
