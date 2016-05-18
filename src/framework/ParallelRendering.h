#ifndef PARALLELRENDERING_H
#define PARALLELRENDERING_H

#include "ProfilingExterns.h"
#include <Headers.hh>
#include <unordered_map>

#include <QObject>
#include <QList>
#include <QSet>
#include <QMap>
#include <QThreadPool>

#include <oclHWDL.h>

#include "TaskRender.h"
#include "TaskCollect.h"
#include "TaskComposite.h"
#include "TaskMakePixmap.h"

#include "CLRenderer.h"
#include "CLCompositorAccumulate.h"
#include "CLAbstractCompositor.h"

#include "CLVolumeVariants.hh"
#include "Transformation.h"




namespace clparen {



typedef QMap< const oclHWDL::Device*, Renderer::CLAbstractRenderer*> CLRenderers;

typedef QMap< const Renderer::CLAbstractRenderer* , Task::TaskRender*> RenderingTasks;

typedef QMap< const Renderer::CLAbstractRenderer* , Task::TaskComposite*> CompositingTasks;

typedef QMap< const Renderer::CLAbstractRenderer* , Task::TaskCollect*> CollectingTasks;

typedef QMap< const Renderer::CLAbstractRenderer* , Task::TaskMakePixmap*> MakePixmapTasks;


/**
 * @brief The ParallelRendering class
 */
class ParallelRendering : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief ParallelRendering
     *
     * @param volume
     * @param frameWidth
     * @param frameHeight
     *  frameWidth and frameHeight will be set for all rendered frames
     *  and collage frame as well.
     */
    ParallelRendering( Volume< uchar >* volume ,
                       const uint frameWidth = 512 ,
                       const uint frameHeight = 512 );



    /**
     * @brief addCLRenderer
     * Create and attach CLRenderer to the GPU indexed by gpuIndex.
     *
     * @param gpuIndex
     */
    virtual void addCLRenderer( const uint64_t gpuIndex );


    /**
     * @brief getCLRenderersCount
     * @return
     */
    virtual int getCLRenderersCount() const ;


    /**
     * @brief addCLCompositor
     * Create and attach CLCompositor to the GPU indexed by gpuIndex.
     * @param gpuIndex
     */
    virtual void addCLCompositor( const uint64_t gpuIndex );


    /**
     * @brief distributeBaseVolume1D
     * Distribute the baseVolume_ over the CLRenderers_ evenly based on
     * the X-axis.
     */
    virtual void distributeBaseVolume1D( );

    /**
     * @brief distributeBaseVolumeWeighted
     */
    virtual void distributeBaseVolumeWeighted();

    /**
     * @brief distributeBaseVolumeMemoryWeighted
     */
    virtual void distributeBaseVolumeMemoryWeighted();

    /**
     * @brief startRendering
     * Spark the rendering loop.
     */
    virtual void startRendering( );



    /**
     * @brief getCLRenderer
     * @param gpuIndex
     * @return
     */
    const Renderer::CLAbstractRenderer &getCLRenderer( const uint64_t gpuIndex ) const ;


    const Compositor::CLAbstractCompositor &getCLCompositor( ) const ;
    /**
     * @brief machineGPUsCount
     * @return
     */
    virtual uint getMachineGPUsCount() const;

    /**
     * @brief getFrameWidth
     * @return
     */
    uint getFrameWidth() const ;

    /**
     * @brief getFrameHeight
     * @return
     */
    uint getFrameHeight() const ;


signals:

    /**
     * @brief frameworkReady_SIGNAL
     */
    void frameworkReady_SIGNAL( );

    /**
     * @brief framesReady_SIGNAL
     * For each rendered frame done, emit a signal.
     */
    void frameReady_SIGNAL( QPixmap *pixmap ,
                            const clparen::Renderer::CLAbstractRenderer * node );

    /**
     * @brief finalFrameReady_SIGNAL
     * For each compositing done, emit a signal and pass
     * the collageFrame.
     *
     * @param finalFrame
     * Composited collage frame as a pixmap.
     */
    void finalFrameReady_SIGNAL( QPixmap *finalFrame );


    /**
     * @brief finishedCompositing_SIGNAL
     */
    void finishedCompositing_SIGNAL( );




public slots :

    /**
     * @brief finishedRendering_SLOT
     * When a CLRenderer finishs rendering, the signal emitted will be
     * mapped to this slot, so it initiates a collecting task to transfer
     * buffers from the rendering GPU to the compositing GPU.
     * @param finishedNode
     */
    void finishedRendering_SLOT( Renderer::CLAbstractRenderer *renderer );

    /**
     * @brief compositingFinished_SLOT
     * When a CLCompositor finishs compositing, the signal emitted will be
     * mapped to this slot, where proper routines will be performed.
     */
    void compositingFinished_SLOT( );

    /**
     * @brief frameLoadedToDevice_SLOT
     * When a collecting task finishs transfering buffers from the rendering
     * GPU to the compositing GPU, it emits a signal mapped to this slot,
     * so this slot initiates a compositing task.
     * @param finishedNode
     */
    void frameLoadedToDevice_SLOT( Renderer::CLAbstractRenderer *renderer );


    /**
     * @brief pixmapReady_SLOT
     * When a thread is done with converting a raw frame to pixmap a signal
     * emitted will be mapped to this slot.
     * @param pixmap
     * produced pixmap.
     * @param node
     * JUST IDENTIFIER for whom this pixmap belongs!
     * if it is nullptr then it belongs to the CompositorNode,
     * otherwise, it belongs to CLRenderer referenced by the pointer.
     */
    void pixmapReady_SLOT( QPixmap *pixmap ,
                           const Renderer::CLAbstractRenderer * renderer );

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
     * @brief updateTranslationZ_SLOT
     * @param distance
     */
    void updateTranslationZ_SLOT( int distance );

    /**
     * @brief updateScaleX_SLOT
     * @param distance
     */
    void updateScaleX_SLOT( int distance );
    /**
     * @brief updateScaleY_SLOT
     * @param distance
     */
    void updateScaleY_SLOT( int distance );
    /**
     * @brief updateScaleZ_SLOT
     * @param distance
     */
    void updateScaleZ_SLOT( int distance );
    /**
     * @brief updateScaleXYZ_SLOT
     * @param distance
     */
    void updateScaleXYZ_SLOT( int distance );
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

    /**
     * @brief activateRenderingKernel_SLOT
     */
    /**
     * @brief updateIsoValue_SLOT
     */
    void updateIsoValue_SLOT( float value );
    /**
     * @brief activateRenderingKernel_SLOT
     * @param type
     */
    void activateRenderingKernel_SLOT( clparen::clKernel::RenderingMode type );



protected:
    /**
     * @brief applyTransformation
     * Start rendering and apply the desired transformation.
     */
    virtual void applyTransformation_();

    /**
     * @brief syncTransformation
     * Copy the transformation parameters from the mutable variables
     * everytime to the mutable parameters when threads are inactive.
     */
    void syncTransformation_();

    /**
     * @brief benchmark_
     */
    void benchmark_() ;

protected :
    //oclHWDl utilities
    /**
     * @brief clHardware_
     */
    oclHWDL::Hardware               clHardware_;

    /**
     * @brief inUseGPUs_
     */
    QSet< const oclHWDL::Device* >  inUseGPUs_;

    /**
     * @brief listGPUs_
     */
    oclHWDL::Devices                listGPUs_;

private:
    /**
     * @brief renderers_
     */
    CLRenderers renderers_;

    /**
     * @brief compositor_
     */
    Compositor::CLAbstractCompositor *compositor_;


protected:

    //threadpools
    /**
     * @brief rendererPool_
     */
    QThreadPool rendererPool_  ; //[producer] for collector pool.

    /**
     * @brief compositorPool_
     */
    QThreadPool compositorPool_; //[consumer] for collector pool.

    /**
     * @brief collectorPool_
     */
    QThreadPool collectorPool_ ; //[producer] for renderer pool AND
                                 //[consumer] for renderer pool.
    /**
     * @brief pixmapMakerPool_
     */
    QThreadPool pixmapMakerPool_;

    //QRunnables to be executed concurrently.
    RenderingTasks  renderingTasks_ ;
    CollectingTasks collectingTasks_ ;
    CompositingTasks compositingTasks_ ;
    Task::TaskMakePixmap *finalFramePixmapTask_;
    MakePixmapTasks makePixmapTasks_ ;

    //Volume Data
    Volume<uchar> *baseVolume_;

    //Transformations
    Transformation transformation_ ;

    //shared data for multithreads, must not be modified during
    //the activity rendering threads.
    //modified using syncTransformation_()
    Transformation transformationAsync_ ;

    //flags
    bool pendingTransformations_;
    bool renderersReady_;

    //counters
    uint8_t activeRenderers_;
    uint8_t readyPixmapsCount_;
    uint8_t compositedFramesCount_ ;

    //facts
    uint machineGPUsCount_;
    const uint frameWidth_ ;
    const uint frameHeight_ ;
};

}
#endif // PARALLELRENDERING_H
