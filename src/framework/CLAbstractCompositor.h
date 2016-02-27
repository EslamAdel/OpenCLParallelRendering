#ifndef CLABSTRACTCOMPOSITOR_H
#define CLABSTRACTCOMPOSITOR_H

#include <Headers.hh>
#include <QVector>

#include "oclHWDL.h"
#include "CLRenderer.h"
#include "Logger.h"
#include "CLFrameVariants.hh"
#include "ProfilingExterns.h"

class CLAbstractCompositor : public QObject
{
    Q_OBJECT

public:
    CLAbstractCompositor( const uint64_t gpuIndex  );

    virtual void allocateFrame( CLRenderer *renderer ) = 0 ;

    virtual void collectFrame( CLRenderer *renderer ,
                               const cl_bool block ) = 0;

    virtual void composite( ) = 0 ;

    virtual void loadFinalFrame( ) = 0 ;

    virtual CLFrameVariant &getFinalFrame() = 0 ;

    uint64_t getGPUIndex( ) const;

    bool readOutReady( ) const ;

private:
    /**
     * @brief selectGPU_
     */
    void selectGPU_( );

    /**
     * @brief initializeContext_
     */
    void initializeContext_( ) ;

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

    const uint64_t gpuIndex_;

    cl_platform_id platform_;

    cl_device_id device_;

    cl_context context_;

    cl_command_queue commandQueue_ ;

    QVector< CLRenderer *> renderers_ ;

    CLFrameVariant finalFrameVariant_ ;

    bool readOutReady_ ;
};

#endif // CLABSTRACTCOMPOSITOR_H
