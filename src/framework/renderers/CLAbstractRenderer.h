#ifndef CLABSTRACTRENDERER_H
#define CLABSTRACTRENDERER_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <oclHWDL.h>

#include "CLData.hh"
#include "CLKernel.hh"


#define LOCAL_SIZE_X    16
#define LOCAL_SIZE_Y    16


namespace clparen {
namespace Renderer{



class CLAbstractRenderer : public QObject
{
    Q_OBJECT
public:

    /**
     * @brief CLAbstractRenderer
     * @param gpuIndex
     * @param frameWidth
     * @param frameHeight
     * @param kernelDirectory
     * @param parent
     */
    explicit CLAbstractRenderer(
            const uint64_t gpuIndex ,
            const Dimensions2D frameDimensions  ,
            const CLData::FRAME_CHANNEL_ORDER frameChannelOrder ,
            const std::string kernelDirectory  = DEFAULT_KERNELS_DIRECTORY ,
            QObject *parent = 0 );


    /**
     * @brief setFrameIndex
     * @param frameIndex
     */
    virtual void setFrameIndex( const uint frameIndex  ) = 0 ;

    /**
     * @brief loadVolume
     * @param volume
     */
    virtual void loadVolume( VolumeVariant &volume ) = 0 ;

    /**
     * @brief getGPUIndex
     * @return
     */
    virtual uint64_t getGPUIndex( ) const ;

    /**
     * @brief getGPUName
     * @return
     */
    std::string getGPUName( ) const ;

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
     * @brief applyTransformation
     */
    virtual void applyTransformation() = 0 ;


    /**
     * @brief getCLFrame
     * @return
     */
    virtual const CLData::CLFrameVariant &getCLFrame( ) const = 0;

    /**
     * @brief getCurrentCenter
     * @return
     */
    virtual const Coordinates3D &getCurrentCenter() const = 0 ;

    /**
     * @brief getFrameIndex
     * @return
     */
    virtual uint getFrameIndex( ) const = 0 ;


    /**
     * @brief switchRenderingKernel
     * @param type
     */
    void switchRenderingKernel( const CLKernel::RenderingMode type );


    /**
     * @brief lessThan
     * @param lhs
     * @param rhs
     * @return
     */
    static bool lessThan( const CLAbstractRenderer* lhs ,
                          const CLAbstractRenderer* rhs );

    /**
     * @brief getRenderingTime
     * @return
     */
    double getRenderingTime();

    /**
     * @brief getFrameDimensions
     * @return
     */
    const Dimensions2D &getFrameDimensions() const;

    /**
     * @brief getSortFirstOffset
     * @return
     */
    const Dimensions2D &getSortFirstOffset() const;

    /**
     * @brief getSortFirstDimensions
     * @return
     */
    const Dimensions2D &getSortFirstDimensions() const;


    /**
     * @brief setSortFirstSettings
     * @param sortFirstOffset
     * @param sortFirstDimensions
     */
    void setSortFirstSettings( const Dimensions2D sortFirstOffset ,
                               const Dimensions2D sortFirstDimensions );


    virtual CLData::FRAME_CHANNEL_ORDER getFrameChannelOrder( ) const = 0;

    /**
     * @brief isRenderingModeSupported
     * @param mode
     * @return
     */
    virtual bool isRenderingModeSupported( CLKernel::RenderingMode mode ) = 0;


    /**
     * @brief updateTransferFunction
     * @param transferFunction
     */
    virtual void updateTransferFunction(
            float *transferFunction , uint length  ) = 0 ;

protected:

    /**
     * @brief renderFrame
     */
    virtual void renderFrame( ) = 0 ;

    /**
     * @brief createPixelBuffer_
     */
    virtual void createPixelBuffer_( ) = 0;

    /**
     * @brief initializeKernels_
     */
    virtual void initializeKernels_( ) = 0;

    /**
     * @brief freeBuffers_
     */
    virtual void freeBuffers_( ) = 0 ;

    /**
     * @brief calculateExecutionTime_
     */
    void calculateExecutionTime_();


    /**
     * @brief defaultRenderingMode_
     * @return
     */
    virtual CLKernel::RenderingMode defaultRenderingMode_() const;
signals:
    /**
     * @brief finishedRendering
     * Inform the outside world if rendering is finished.
     * @param thisPtr
     * Pass this pointer as Identifier to this node.
     */
    void finishedRendering( Renderer::CLAbstractRenderer *thisPtr );


public slots:


private:
    /**
     * @brief initializeContext_
     */
    void initializeContext_( );

    /**
     * @brief selectGPU_
     */
    void selectGPU_( );

    /**
     * @brief createCommandQueue_
     */
    void createCommandQueue_( );

    /**
     * @brief allocateKernels_
     * @return
     */
    CLKernel::CLRenderingKernels allocateKernels_( ) const;
protected:

    /**
     * @brief kernelDirectory_
     */
    const std::string kernelDirectory_ ;
    /**
     * @brief gpuIndex_
     * Selected GPU, where the volume rendering kernel will be executed.
     */
    const uint64_t gpuIndex_;

    /**
     * @brief name_
     */
    std::string name_ ;

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
     * @brief frameWidth_
     */
    const Dimensions2D frameDimensions_ ;

    /**
     * @brief sortFirstOffset_
     */
    Dimensions2D sortFirstOffset_ ;

    /**
     * @brief sortFirstDimensions_
     */
    Dimensions2D sortFirstDimensions_;

    /**
     * @brief frameVariant_
     */
    mutable CLData::CLFrameVariant frameVariant_ ;


    /**
     * @brief switchKernelMutex_
     */
    QMutex switchKernelMutex_ ;

    /**
     * @brief renderingKernels_
     */
    CLKernel::CLRenderingKernels renderingKernels_ ;

    /**
     * @brief activeRenderingKernel_
     */
    CLKernel::CLRenderingKernel* activeRenderingKernel_;

    /**
     * @brief activeRenderingMode_
     */
    CLKernel::RenderingMode activeRenderingMode_ ;

    /**
     * @brief GPUExecution_
     */
    cl_event clGPUExecution_;

    /**
     * @brief renderingTime_
     */
    double renderingTime_ ;

    /**
     * @brief frameChannelOrder_
     */
    const CLData::FRAME_CHANNEL_ORDER frameChannelOrder_ ;
};


}}

#endif // CLABSTRACTRENDERER_H
