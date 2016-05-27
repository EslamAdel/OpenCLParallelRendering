#ifndef CLIsoSurfaceRENDERINGKERNEL_H
#define CLIsoSurfaceRENDERINGKERNEL_H

#include "CLRenderingKernel.h"


namespace clparen  {
namespace CLKernel {


class CLIsoSurfaceRenderingKernel : public CLRenderingKernel
{
private:
    enum XRayKernelArgument
    {
        KERNEL_ARG_Density = KERNEL_ARG_DerivedKernelsOffset  ,
        KERNEL_ARG_Brightness,
        KERNEL_ARG_IsoValue
    };

public:
    CLIsoSurfaceRenderingKernel(
            cl_context sharedContext = NULL,
            const std::string kernelDirectory = ".",
            const std::string kernelFile = "isoSurface.cl" ,
            const std::string kernelName = "isoSurface" );

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

    /**
     * @brief setVolumeIsoValue
     * @param isovalue
     */
    virtual void setVolumeIsoValue( float isovalue ) final;



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

#endif // CLISOSURFACERENDERINGKERNEL_H
