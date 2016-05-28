#ifndef CLABSTRACTCOMPOSITOR_H
#define CLABSTRACTCOMPOSITOR_H

#include "Headers.hh"
#include <QVector>
#include <QMutex>
#include <QMutexLocker>

#include "oclHWDL.h"
#include "CLRenderer.h"
#include "Logger.h"
#include "CLData.hh"
#include "ProfilingExterns.h"
#include "CLXRayCompositingKernel.h"
#include "CLMaxIntensityProjectionCompositingKernel.h"
#include "CLMinIntensityProjectionCompositingKernel.h"

#define LOCAL_SIZE_X    16
#define LOCAL_SIZE_Y    16


namespace clparen {
namespace Compositor {



class CLAbstractCompositor : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief CLAbstractCompositor
     * @param gpuIndex
     * @param frameWidth
     * @param frameHeight
     * @param kernelDirectory
     * @param parent
     */
    CLAbstractCompositor(
            const uint64_t gpuIndex ,
            const uint frameWidth , const uint frameHeight ,
            const std::string kernelDirectory = DEFAULT_KERNELS_DIRECTORY ,
            QObject *parent = 0  );

    /**
     * @brief allocateFrame
     * @param renderer
     */
    virtual void allocateFrame( Renderer::CLAbstractRenderer *renderer ) = 0 ;

    /**
     * @brief collectFrame
     * @param renderer
     * @param block
     */
    virtual void collectFrame( Renderer::CLAbstractRenderer *renderer ,
                               const cl_bool block ) = 0;

    /**
     * @brief composite
     */
    virtual void composite( ) = 0 ;

    /**
     * @brief loadFinalFrame
     */
    virtual void loadFinalFrame( ) = 0 ;

    /**
     * @brief getFinalFrame
     * @return
     */
    virtual const CLData::CLFrameVariant &getFinalFrame() const = 0 ;

    /**
     * @brief getGPUIndex
     * @return
     */
    uint64_t getGPUIndex( ) const;

    /**
     * @brief readOutReady
     * @return
     */
    bool readOutReady( ) const ;

    /**
     * @brief switchCompositingKernel
     * @param mode
     */
    void switchCompositingKernel( const CLKernel::RenderingMode mode ) ;


    /**
     * @brief isRenderingModeSupported
     * @param mode
     * @return
     */
    virtual bool isRenderingModeSupported( CLKernel::RenderingMode mode ) = 0;
private:
    /**
     * @brief selectGPU_
     */
    void selectGPU_( );

    /**
     * @brief initializeContext_
     */
    void initializeContext_( ) ;

    /**
     * @brief allocateKernels_
     * @return
     */
    CLKernel::CLCompositingKernels allocateKernels_( ) const;

protected:
    /**
     * @brief initializeBuffers_
     * Allocate framesCount_ buffers on the compositng device, in addition
     * to the collage buffer.
     */
    virtual void initializeBuffers_( ) = 0 ;

    /**
     * @brief initializeKernel_
     * Constructs the compositing kernel and the rewinding kernel, also
     * the collage buffer is passed to the rewinding kernel, as it will
     * always be rewinded after compositing is completely done.
     */
    virtual void initializeKernel_( ) = 0 ;

private:
    /**
     * @brief createCommandQueue_
     */
    void createCommandQueue_( );

protected:

    /**
     * @brief frameDimensions_
     */
    const Dimensions2D frameDimensions_;

    /**
     * @brief kernelDirectory_
     */
    const std::string kernelDirectory_ ;

    /**
     * @brief gpuIndex_
     */
    const uint64_t gpuIndex_;

    /**
     * @brief platform_
     */
    cl_platform_id platform_;

    /**
     * @brief device_
     */
    cl_device_id device_;

    /**
     * @brief context_
     */
    cl_context context_;

    /**
     * @brief commandQueue_
     */
    cl_command_queue commandQueue_ ;

    /**
     * @brief renderers_
     */
    QVector< const Renderer::CLAbstractRenderer *> renderers_ ;

    /**
     * @brief finalFrameVariant_
     */
    mutable CLData::CLFrameVariant finalFrameVariant_ ;

    /**
     * @brief readOutReady_
     */
    bool readOutReady_ ;

    /**
     * @brief switchKernelMutex_
     */
    QMutex switchKernelMutex_ ;

    /**
     * @brief renderingKernels_
     */
    CLKernel::CLCompositingKernels compositingKernels_ ;

    /**
     * @brief activeRenderingKernel_
     */
    CLKernel::CLCompositingKernel* activeCompositingKernel_;
};

}
}

#endif // CLABSTRACTCOMPOSITOR_H
