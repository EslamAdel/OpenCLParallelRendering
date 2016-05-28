#ifndef CLRENDERINGKERNEL_H
#define CLRENDERINGKERNEL_H

#include "Headers.h"
#include <oclHWDL/oclHWDL.h>
#include <QMap>
#include <QSet>
#include <QCoreApplication>
#include <QDir>
#include "CLFrame.h"
#define DEFAULT_KERNELS_DIRECTORY \
    QString( qApp->applicationDirPath() + \
    QDir::separator() + \
    ".." + \
    QDir::separator() + \
    "share" ).toStdString()


namespace clparen {
namespace CLKernel {

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
        KERNEL_ARG_SortFirstWidth ,
        KERNEL_ARG_SortFirstHeight ,
        KERNEL_ARG_InverseMatrix ,
        KERNEL_ARG_Volume ,
        KERNEL_ARG_VolumeSampler ,
        KERNEL_ARG_Density ,
        KERNEL_ARG_Brightness ,
        KERNEL_ARG_DerivedKernelsOffset
    };


public:

    /**
     * @brief CLRenderingKernel
     * @param clContext
     * @param kernelDirectory
     * @param kernelFile
     */
    CLRenderingKernel(
            cl_context clContext ,
            const std::string kernelName ,
            const std::string kernelFile ,
            const std::string kernelDirectory = DEFAULT_KERNELS_DIRECTORY );



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
    virtual RenderingMode getRenderingKernelType( ) const;

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
     * @brief setSortFirstWidth
     * @param width
     */
    void setSortFirstWidth( uint width );

    /**
     * @brief setSortFirstHeight
     * @param height
     */

    void setSortFirstHeight( uint height );
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
    virtual void setVolumeDensityFactor( float density ) ;

    /**
     * @brief setImageBrightnessFactor
     * @param brightness
     */

    virtual void setImageBrightnessFactor( float brightness );
    /**
     * @brief setVolumeIsoValue
     * @param isovalue
     */
    virtual void setVolumeIsoValue( float isovalue ) ;

    /**
     * @brief setMaxSteps
     * @param maxSteps
     */
    virtual void setMaxSteps( uint maxSteps );

    /**
     * @brief setStepSize
     * @param tStep
     */
    virtual void setStepSize( float tStep );


    /**
     * @brief setTransferFunctionData
     * @param data
     */
    virtual void setTransferFunctionData( cl_mem data );

    /**
     * @brief setTransferFunctionSampler
     * @param sampler
     */
    virtual void setTransferFunctionSampler( cl_sampler sampler );


    /**
     * @brief releaseKernel
     */
    void releaseKernel( );


    virtual bool isFramePrecisionSupported(
            CLData::FRAME_CHANNEL_TYPE precision ) = 0;

protected:

    /**
     * @brief buildKernel_
     * @param extraBuildOptions
     */
    virtual void buildKernel_( const std::string extraBuildOptions = "" );

    /**
     * @brief retrieveKernelObject_
     */
    void retrieveKernelObject_( );

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
     * @brief kernelName_
     */
    const std::string kernelName_;

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


    static const QSet< CLData::FRAME_CHANNEL_TYPE > framePrecision_RGBA_ ;

    static const QSet< CLData::FRAME_CHANNEL_TYPE > framePrecision_Luminance_;
};

/**
 * @brief CLRenderingKernels
 */
typedef QMap< RenderingMode , CLRenderingKernel* > CLRenderingKernels;


}
}

#endif // CLRENDERINGKERNEL_H
