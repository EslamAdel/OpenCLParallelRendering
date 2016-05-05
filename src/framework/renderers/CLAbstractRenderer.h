#ifndef CLABSTRACTRENDERER_H
#define CLABSTRACTRENDERER_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <oclHWDL.h>

#include "CLFrameVariants.hh"
#include "CLVolumeVariants.hh"
#include <CLRenderingKernel.h>
#include <CLXRayRenderingKernel.h>

class CLAbstractRenderer : public QObject
{
    Q_OBJECT
public:
    explicit CLAbstractRenderer( const uint64_t gpuIndex ,
                                 const uint frameWidth , const uint frameHeight ,
                                 const std::string kernelDirectory  ,
                                 QObject *parent = 0 );


    virtual void setFrameIndex( const uint frameIndex  ) = 0 ;

    virtual void loadVolume( const VolumeVariant &volume ) = 0 ;

    virtual uint64_t getGPUIndex( ) const ;

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


    virtual void applyTransformation() = 0 ;


    virtual const CLFrameVariant &getCLFrame( ) const = 0;

    virtual const Coordinates3D &getCurrentCenter() const = 0 ;

    virtual uint getFrameIndex( ) const = 0 ;


    void switchRenderingKernel( const RenderingMode type ) ;


    static bool lessThan( const CLAbstractRenderer* lhs ,
                          const CLAbstractRenderer* rhs );

protected:

    virtual void renderFrame( ) = 0 ;

    virtual void createPixelBuffer_( ) = 0;

    virtual void initializeKernels_( ) = 0;

    virtual void freeBuffers_( ) = 0 ;

signals:
    /**
     * @brief finishedRendering
     * Inform the outside world if rendering is finished.
     * @param thisPtr
     * Pass this pointer as Identifier to this node.
     */
    void finishedRendering( CLAbstractRenderer *thisPtr );


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
    CLRenderingKernels allocateKernels_( ) const;
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
    const uint frameWidth_ ;

    /**
     * @brief frameHeight_
     */
    const uint frameHeight_ ;

    /**
     * @brief frameVariant_
     */
    mutable CLFrameVariant frameVariant_ ;


    /**
     * @brief switchKernelMutex_
     */
    QMutex switchKernelMutex_ ;

    /**
     * @brief renderingKernels_
     */
    const CLRenderingKernels renderingKernels_ ;

    /**
     * @brief activeRenderingKernel_
     */
    CLRenderingKernel* activeRenderingKernel_;

};

#endif // CLABSTRACTRENDERER_H
