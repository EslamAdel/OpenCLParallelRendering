#ifndef CLGPUCONTEXT_H
#define CLGPUCONTEXT_H

#include <Headers.hh>
#include <Volume.h>
#include <oclHWDL/oclHWDL.h>
#include <CLVolume.h>

#include <CLXRayRenderingKernel.h>

/**
 * @brief The DISPLAY_MODE enum
 */
enum DISPLAY_MODE
{
    // Don't display the resulting frame with OpenGL
    NO_DISPLAY,

    // Use the interoperability between OpenCL and OpenGL
    CL_GL_INTEROPERABILITY,

    // Read back the frame from CL and display it withe OpenGL
    CL_GL_READBACK
};

/**
 * @brief The CLGPUContext class
 */
template < class T >
class CLGLContext
{
public:

    CLGLContext( const Volume< T >* volume,
                  const uint64_t gpuIndex,
                  const DISPLAY_MODE displayMode );
    ~CLGLContext( );

public:

    /**
     * @brief registerGLPixelBuffer
     * @param glPixelBuffer
     * @param bufferWidth
     * @param bufferHeight
     */
    void registerGLPixelBuffer( const uint glPixelBuffer,
                                const uint bufferWidth,
                                const uint bufferHeight );

    /**
     * @brief createPixelBuffer
     * @param bufferSize
     */
    void createPixelBuffer( const size_t bufferSize );

    /**
     * @brief getGPUIndex
     * @return
     */
    uint64_t getGPUIndex( ) const;

    /**
     * @brief getPlatformId
     * @return
     */
    cl_platform_id getPlatformId( ) const;

    /**
     * @brief getDeviceId
     * @return
     */
    cl_device_id getDeviceId( ) const;

    /**
     * @brief getContext
     * @return
     */
    cl_context getContext( ) const;

    /**
     * @brief getCommandQueue
     * @return
     */
    cl_command_queue getCommandQueue( ) const;


    /**
      * @brief getKernel
      * @return
      */
    cl_kernel getKernel( ) const;

    /**
      * @brief updateGridSize
      * @param width
      * @param height
      */
    void updateGridSize( const uint64_t width, const uint64_t height );

    /**
      * @brief updateImageBrightness
      * @param imageBrightnessValue
      */
    void updateImageBrightness( const float imageBrightnessValue );

    /**
      * @brief updateVolumeDensity
      * @param volumeDensityValue
      */
    void updateVolumeDensity( const float volumeDensityValue );

    /**
      * @brief updateTransferFunctionScale
      * @param tfScaleValue
      */
    void updateTransferFunctionScale( const float tfScaleValue );

    /**
      * @brief updateTransferFunctionOffset
      * @param tfOffsetValue
      */
    void updateTransferFunctionOffset( const float tfOffsetValue );

    void handleKernel(std::string string = "");

    /**
      * @brief renderFrame
      * @param inverseMatrix_
      */
    void renderFrame( float* inverseMatrix );

    /**
      * @brief releasePixelBuffer
      */
    void releasePixelBuffer( );

private:

    /**
     * @brief initalizeContext_
     */
    void initalizeContext_( );

    /**
     * @brief selectGPU_
     */
    void selectGPU_( );

    /**
     * @brief createCommandQueue_
     */
    void createCommandQueue_( );

    /**
     * @brief freeBuffers_
     */
    void freeBuffers_( );


private:

    /**
     * @brief volume_
     */
    const Volume< T >* volume_;

    /**
     * @brief clVolume_
     */
    CLVolume< T >* clVolume_;

    /**
     * @brief gpuIndex_
     * Selected GPU, where the volume rendering kernel will be executed.
     */
    const uint64_t gpuIndex_;

    /**
     * @brief displayMode_
     */
    const DISPLAY_MODE displayMode_;

    /**
     * @brief platform_
     * Selected platform, where the volume rendering kernel will be executed.
     */
    cl_platform_id platform_;

    /**
     * @brief device_
     * Selected GPU device, where the volume rendering kernel will be executed.
     */
    cl_device_id device_;

    /**
     * @brief context_
     * OpenCL execution context
     */
    cl_context context_;

    /**
     * @brief commandQueue_
     */
    cl_command_queue commandQueue_;

    /**
     * @brief kernelContext_
     */
    oclHWDL::KernelContext* kernelContext_;

    /**
     * @brief kernel_
     */
    cl_kernel kernel_;

    /**
     * @brief clPixelBuffer_
     */
    cl_mem clPixelBuffer_;

    /**
     * @brief inverseMatrix_
     */
    cl_mem inverseMatrix_;

    /**
     * @brief volumeArray_
     */
    cl_mem volumeArray_;

    /**
     * @brief transferFunctionArray_
     */
    cl_mem transferFunctionArray_;

    /**
     * @brief linearVolumeSampler_
     */
    cl_sampler linearVolumeSampler_;

    /**
     * @brief nearestVolumeSampler_
     */
    cl_sampler nearestVolumeSampler_;

    /**
     * @brief transferFunctionSampler_
     */
    cl_sampler transferFunctionSampler_;

    /**
     * @brief volumeDensity_
     */
    float volumeDensity_;

    /**
     * @brief imageBrightness_
     */
    float imageBrightness_;

    /**
     * @brief tfOffset_
     */
    float tfOffset_;

    /**
     * @brief tfScale_
     */
    float tfScale_;

    /**
     * @brief linearFiltering_
     */
    bool linearFiltering_;

    /**
     * @brief gridSize_
     */
    size_t gridSize_[2];

    /**
     * @brief renderingKernels_
     */
    std::vector< CLRenderingKernel* > renderingKernels_;

    /**
     * @brief activeRenderingKernel_
     */
    CLRenderingKernel* activeRenderingKernel_;
};

#endif // CLGPUCONTEXT_H
