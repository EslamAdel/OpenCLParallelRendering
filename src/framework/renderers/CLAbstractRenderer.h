#ifndef CLABSTRACTRENDERER_H
#define CLABSTRACTRENDERER_H

#include <QObject>
#include <oclHWDL.h>

#include "CLFrameVariants.hh"
#include "CLVolumeVariants.hh"

class CLAbstractRenderer : public QObject
{
    Q_OBJECT
public:
    explicit CLAbstractRenderer( const uint64_t gpuIndex ,
                                 const uint frameWidth , const uint frameHeight ,
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
    /**
      * @brief getKernel
      * @return
      */
    virtual cl_kernel getKernel( ) const = 0;

    virtual const CLFrameVariant &getCLFrame( ) const = 0;

    virtual const Coordinates3D &getCurrentCenter() const = 0 ;

    virtual uint getFrameIndex( ) const = 0 ;

    static bool lessThan( const CLAbstractRenderer* lhs ,
                          const CLAbstractRenderer* rhs );

protected:

    virtual void renderFrame( ) = 0 ;

    virtual void createPixelBuffer_( ) = 0;

    virtual void initializeKernel_( ) = 0;

    virtual void handleKernel_(std::string string = "") = 0 ;

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

protected:
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

    mutable CLFrameVariant frameVariant_ ;
};

#endif // CLABSTRACTRENDERER_H
