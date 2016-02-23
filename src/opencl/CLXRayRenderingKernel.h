#ifndef CLXRAYRENDERINGKERNEL_H
#define CLXRAYRENDERINGKERNEL_H

#include <CLRenderingKernel.h>

class CLXRayRenderingKernel : public CLRenderingKernel
{


public:
    CLXRayRenderingKernel( cl_context clContext = NULL,
                           const std::string kernelDirectory = ".",
                           const std::string kernelFile = "xray.cl" ,
                           const std::string kernelName = "xray" );

public:

    RENDERING_KERNEL_TYPE getRenderingKernelType( ) const;

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
     * @brief setVolumeDensityFactor
     * @param density
     */
    void setVolumeDensityFactor( float density );

    /**
     * @brief setImageBrightnessFactor
     * @param brightness
     */
    void setImageBrightnessFactor( float brightness );

    /**
     * @brief setInverseViewMatrix
     * @param matrix
     */
    void setInverseViewMatrix( cl_mem matrix );

    /**
     * @brief setTransferFunctionData
     * @param data
     */
    void setTransferFunctionData(cl_mem data);

    /**
     * @brief setTransferFunctionOffset
     * @param offset
     */
    void setTransferFunctionOffset(float offset);

    /**
     * @brief setTransferFunctionSampler
     * @param sampler
     */
    void setTransferFunctionSampler(cl_sampler sampler);

    /**
     * @brief setTransferFunctionScale
     * @param scale
     */
    void setTransferFunctionScale(float scale);

    /**
     * @brief setTransferFunctionFlag
     * @param enableTransferFunction
     */
    void setTransferFunctionFlag(int enableTransferFunction);

private:

    /**
     * @brief retrieveKernelObject_
     */
    void retrieveKernelObject_( );

private:

    /**
     * @brief kernelName_
     */
    const std::string kernelName_;
};

#endif // CLXRAYRENDERINGKERNEL_H
