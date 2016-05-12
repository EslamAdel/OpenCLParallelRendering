#ifndef CLRENDERINGKERNEL_H
#define CLRENDERINGKERNEL_H

#include <Headers.hh>
#include <oclHWDL/oclHWDL.h>
#include <QMap>





namespace clparen {
namespace clKernel {

enum RenderingMode
{
    RENDERING_MODE_Xray = 0 ,
    RENDERING_MODE_MinIntensity ,
    RENDERING_MODE_MaxIntensity ,
    RENDERING_MODE_AverageIntensity ,
    RENDERING_MODE_IsoSurface ,
    RENDERING_MODE_AlphaBlending
};




class CLRenderingKernel
{

    Q_DISABLE_COPY( CLRenderingKernel )

protected:
    enum KernelArgument
    {
        KERNEL_ARG_FrameBuffer = 0 ,
        KERNEL_ARG_FrameWidth ,
        KERNEL_ARG_FrameHeight ,
        KERNEL_ARG_InverseMatrix ,
        KERNEL_ARG_Volume ,
        KERNEL_ARG_VolumeSampler ,
        KERNEL_ARG_DerivedKernelsOffset
    };


public:

    /**
     * @brief CLRenderingKernel
     * @param clContext
     * @param kernelDirectory
     * @param kernelFile
     */
    CLRenderingKernel( cl_context clContext ,
                       const std::string kernelDirectory,
                       const std::string kernelFile );
    ~CLRenderingKernel( );

public:

    /**
      * @brief getKernelDirectory
      * @return
      */
     std::string getKernelDirectory( ) const;

    /**
     * @brief getKernelFile
     * @return
     */
    std::string getKernelFile( ) const;

    /**
     * @brief getKernelContext
     * @return
     */
    oclHWDL::KernelContext* getKernelContext( ) const;

    /**
     * @brief getKernelObject
     * @return
     */
    cl_kernel getKernelObject( ) const;

    /**
     * @brief getRenderingKernelType
     * @return
     */
    virtual RenderingMode getRenderingKernelType( ) const = 0;

    /**
     * @brief setFrameBuffer
     * @param frameBuffer
     */
    void setFrameBuffer( cl_mem frameBuffer );

    /**
     * @brief setFrameWidth
     * @param width
     */
    void setFrameWidth( uint width );

    /**
     * @brief setFrameHeight
     * @param height
     */
    void setFrameHeight( uint height );

    /**
     * @brief setVolumeData
     * @param data
     */
    void setVolumeData( cl_mem data );

    /**
     * @brief setVolumeSampler
     * @param sampler
     */
    void setVolumeSampler( cl_sampler sampler );

    /**
     * @brief setInverseViewMatrix
     * @param matrix
     */
    void setInverseViewMatrix( cl_mem matrix );

    /**
     * @brief setVolumeDensityFactor
     * @param density
     */
    virtual void setVolumeDensityFactor( float density ) = 0;

    /**
     * @brief setImageBrightnessFactor
     * @param brightness
     */
    virtual void setImageBrightnessFactor( float brightness ) = 0;


    /**
     * @brief releaseKernel
     */
    void releaseKernel( );

protected:

    /**
     * @brief buildKernel_
     * @param extraBuildOptions
     */
    void buildKernel_( const std::string extraBuildOptions = "" );

    /**
     * @brief retrieveKernelObject_
     */
    virtual void retrieveKernelObject_( ) = 0;

protected:

    /**
     * @brief kernelDirectory_
     */
    const std::string kernelDirectory_;

    /**
     * @brief kernelFile_
     */
    const std::string kernelFile_;

    /**
     * @brief clContext_
     */
    cl_context clContext_;

    /**
     * @brief kernelContext_
     */
    oclHWDL::KernelContext* kernelContext_;

    /**
      * @brief kernelObject_
      */
     cl_kernel kernelObject_;

     /**
      * @brief kernelType_
      */
     RenderingMode kernelType_;

};

/**
 * @brief CLRenderingKernels
 */
typedef QMap< RenderingMode , CLRenderingKernel* > CLRenderingKernels;


}
}

#endif // CLRENDERINGKERNEL_H
