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

    void loadNewVolume( const Volume< T >* volume );

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
    void renderFrame( const float* inverseMatrix ,
                      const float &volumeDensity ,
                      const float &imageBrightness );


    void uploadFrame();

    void frameBufferToPixmap();

    /**
      * @brief releasePixelBuffer
      */
    void releasePixelBuffer( );

    /**
     * @brief getFrame
     * @return
     */
    QPixmap* getFrame( );


    void paint( const Coordinates3D &rotation ,
                const Coordinates3D &translation,
                const float &volumeDensity ,
                const float &imageBrightness);

    bool kernelInitialized() const;

private:

    /**
     * @brief initalizeContext_
     */
    virtual void initializeContext_( );

    virtual void initializeKernel_( );

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
     * @brief inverseMatrix_
     */
    float inverseMatrixArray_[ 12 ];

    bool kernelInitialized_;
};

#endif // CLGPUCONTEXT_H