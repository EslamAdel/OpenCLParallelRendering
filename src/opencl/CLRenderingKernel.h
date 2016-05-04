#ifndef CLRENDERINGKERNEL_H
#define CLRENDERINGKERNEL_H

#include <Headers.hh>
#include <oclHWDL/oclHWDL.h>

enum RENDERING_KERNEL_TYPE
{
    ALPHA_BLENDEING,
    X_RAY,
    MAXIMUM_INTENSITY_PROJECTION,
    MINIMUM_INTENSITY_PROJECTION,
    AVERAGE_INTENSITY_PROJECTION,
    ISO_SURFACE
};

class CLRenderingKernel
{
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
    virtual RENDERING_KERNEL_TYPE getRenderingKernelType( ) const = 0;

    /**
     * @brief setFrameBuffer
     * @param frameBuffer
     */
    virtual void setFrameBuffer( cl_mem frameBuffer ) = 0;

    /**
     * @brief setFrameWidth
     * @param width
     */
    virtual void setFrameWidth( uint width ) = 0;

    /**
     * @brief setFrameHeight
     * @param height
     */
    virtual void setFrameHeight( uint height ) = 0;

    /**
     * @brief setVolumeData
     * @param data
     */
    virtual void setVolumeData( cl_mem data ) = 0;

    /**
     * @brief setVolumeSampler
     * @param sampler
     */
    virtual void setVolumeSampler( cl_sampler sampler ) = 0;

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
     * @brief setInverseViewMatrix
     * @param matrix
     */
    virtual void setInverseViewMatrix( cl_mem matrix ) = 0;

//    /**
//     * @brief setTransferFunctionData
//     * @param data
//     */
//    virtual void setTransferFunctionData( cl_mem data );

//    /**
//     * @brief setTransferFunctionSampler
//     * @param sampler
//     */
//    virtual void setTransferFunctionSampler( cl_sampler sampler );

//    /**
//     * @brief setTransferFunctionOffset
//     * @param offset
//     */
//    virtual void setTransferFunctionOffset( float offset );

//    /**
//     * @brief setTransferFunctionScale
//     * @param scale
//     */
//    virtual void setTransferFunctionScale( float scale );

//    /**
//     * @brief setTransferFunctionFlag
//     * @param enableTransferFunction
//     */
//    virtual void setTransferFunctionFlag(int enableTransferFunction);

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
     RENDERING_KERNEL_TYPE kernelType_;
};

/**
 * @brief CLRenderingKernels
 */
typedef std::vector< CLRenderingKernel* > CLRenderingKernels;

#endif // CLRENDERINGKERNEL_H
