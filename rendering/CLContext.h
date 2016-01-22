#ifndef CLGPUCONTEXT_H
#define CLGPUCONTEXT_H

#include <Headers.hh>
#include <Volume.h>
#include <oclHWDL/oclHWDL.h>
#include <QPixmap>
#include <CLVolume.h>
#include <CLXRayRenderingKernel.h>

/**
 * @brief The CLContext class
 */
template < class T >
class CLContext
{
public:

    /**
     * @brief CLContext
     * @param volume
     * @param gpuIndex
     */
    CLContext( const Volume< T >* volume, const uint64_t gpuIndex );
    ~CLContext( );

public:
    /**
     * @brief createPixelBuffer
     * @param bufferSize
     */
    void createPixelBuffer( const uint bufferWidth,
                            const uint bufferHeight );

    /**
     * @brief startRendering
     */
    void startRendering( );

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
     * @brief handleKernel
     * @param string
     */
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

    /**
     * @brief getFrame
     * @return
     */
    QPixmap* getFrame( );

public slots:

    /**
     * @brief updateRotationX_SLOT
     * @param angle
     */
    void updateRotationX_SLOT( int angle );

    /**
     * @brief updateRotationY_SLOT
     * @param angle
     */
    void updateRotationY_SLOT( int angle );

    /**
     * @brief updateRotationZ_SLOT
     * @param angle
     */
    void updateRotationZ_SLOT( int angle );

    /**
     * @brief updateTranslationX_SLOT
     * @param distance
     */
    void updateTranslationX_SLOT( int distance );

    /**
     * @brief updateTranslationY_SLOT
     * @param distance
     */
    void updateTranslationY_SLOT( int distance );

    /**
     * @brief updateImageBrightness_SLOT
     * @param brithness
     */
    void updateImageBrightness_SLOT( float brithness );

    /**
     * @brief updateVolumeDensity_SLOT
     * @param density
     */
    void updateVolumeDensity_SLOT( float density );

    void paint();

private:

    /**
     * @brief initalizeContext_
     */
    virtual void initalizeContext_( );

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

protected:

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
    size_t gridSize_[ 2 ];

    /**
     * @brief renderingKernels_
     */
    std::vector< CLRenderingKernel* > renderingKernels_;

    /**
     * @brief activeRenderingKernel_
     */
    CLRenderingKernel* activeRenderingKernel_;

    /**
     * @brief frameData_
     */
    uint* frameData_;

    /**
     * @brief frameDataRGBA_
     */
    uchar* frameDataRGBA_;

    /**
     * @brief frame_
     */
    QPixmap frame_;

    /**
     * @brief rotation_
     */
    Coordinates3D rotation_;

    /**
     * @brief translation_
     */
    Coordinates3D translation_;

    /**
     * @brief inverseMatrix_
     */
    float inverseMatrixArray_[ 12 ];
};

#endif // CLGPUCONTEXT_H
