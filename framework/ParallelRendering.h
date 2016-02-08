#ifndef PARALLELRENDERING_H
#define PARALLELRENDERING_H

#include "ProfilingExterns.h"
#include <Headers.hh>
#include <unordered_map>

#include <QObject>
#include <QList>
#include <QSet>
#include <QThreadPool>

#include <oclHWDL.h>

#include "TaskRender.h"
#include "TaskCollect.h"
#include "TaskComposite.h"
#include "TaskMakePixmap.h"

#include "RenderingNode.h"
#include "CompositingNode.h"

#include "Volume.h"

typedef std::unordered_map<const oclHWDL::Device*,RenderingNode*> RenderingNodes;
typedef std::unordered_map<const RenderingNode* ,TaskRender*> RenderingTasks;
typedef std::unordered_map<const RenderingNode* ,TaskComposite*> CompositingTasks;
typedef std::unordered_map<const RenderingNode* ,TaskCollect*> CollectingTasks;
typedef std::unordered_map<const RenderingNode* ,TaskMakePixmap*> MakePixmapTasks;

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
    ParallelRendering(Volume< uchar >* volume ,
                      const uint frameWidth = 512,
                      const uint frameHeight = 512 );


    /**
     * @brief discoverAllNodes
     * Create and attache RenderingNodes to the available GPUs on machine.
     */
    void discoverAllNodes();

    /**
     * @brief addRenderingNode
     * Create and attach RenderingNode to the GPU indexed by gpuIndex.
     *
     * @param gpuIndex
     */
    void addRenderingNode( const uint64_t gpuIndex );



    /**
     * @brief addCompositingNode
     * Create and attach CompositingNode to the GPU indexed by gpuIndex.
     * @param gpuIndex
     */
    void addCompositingNode( const uint64_t gpuIndex );

    /**
     * @brief distributeBaseVolume1D
     * Distribute the baseVolume_ over the renderingNodes_ evenly based on
     * the X-axis.
     */
    void distributeBaseVolume1D();

    /**
     * @brief startRendering
     * Spark the rendering loop.
     */
    void startRendering();



    /**
     * @brief getRenderingNode
     * @param gpuIndex
     * @return
     */
    RenderingNode &getRenderingNode( const uint64_t gpuIndex );

    /**
     * @brief machineGPUsCount
     * @return
     */
    uint8_t machineGPUsCount() const;

    /**
     * @brief activeRenderingNodesCount
     * @return
     */
    uint8_t activeRenderingNodesCount() const;


signals:
    /**
     * @brief framesReady_SIGNAL
     * For each rendered frame done, emit a signal.
     */
    void frameReady_SIGNAL( QPixmap *pixmap , const RenderingNode * node );

    /**
     * @brief finalFrameReady_SIGNAL
     * For each compositing done, emit a signal and pass
     * the collageFrame.
     *
     * @param finalFrame
     * Composited collage frame as a pixmap.
     */
    void finalFrameReady_SIGNAL( QPixmap *finalFrame );

public slots :

    /**
     * @brief finishedRendering_SLOT
     * When a RenderingNode finishs rendering, the signal emitted will be
     * mapped to this slot, so it initiates a collecting task to transfer
     * buffers from the rendering GPU to the compositing GPU.
     * @param finishedNode
     */
    void finishedRendering_SLOT( RenderingNode *finishedNode );

    /**
     * @brief compositingFinished_SLOT
     * When a CompositingNode finishs compositing, the signal emitted will be
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
    void frameLoadedToDevice_SLOT( RenderingNode *finishedNode );


    /**
     * @brief pixmapReady_SLOT
     * When a thread is done with converting a raw frame to pixmap a signal
     * emitted will be mapped to this slot.
     * @param pixmap
     * produced pixmap.
     * @param node
     * JUST IDENTIFIER for whom this pixmap belongs!
     * if it is nullptr then it belongs to the CompositorNode,
     * otherwise, it belongs to RenderingNode referenced by the pointer.
     */
    void pixmapReady_SLOT( QPixmap *pixmap , const RenderingNode * node );

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

private:
    /**
     * @brief applyTransformation
     * Start rendering and apply the desired transformation.
     */
    void applyTransformation_();

    /**
     * @brief syncTransformation
     * Copy the transformation parameters from the mutable variables
     * everytime to the mutable parameters when threads are inactive.
     */
    void syncTransformation_();

    void benchmark_() ;

private:

    //oclHWDl utilities
    oclHWDL::Hardware         clHardware_;
    QSet< oclHWDL::Device* >   inUseGPUs_;
    oclHWDL::Devices            listGPUs_;

    //The workers, each node is attached to a single device.
    RenderingNodes        renderingNodes_;
    CompositingNode     *compositingNode_;

    //threadpools
    QThreadPool rendererPool_  ; //[producer] for collector pool.
    QThreadPool compositorPool_; //[consumer] for collector pool.
    QThreadPool collectorPool_ ; //[producer] for renderer pool AND
                                 //[consumer] for renderer pool.
    QThreadPool pixmapMakerPool_;

    //QRunnables to be executed concurrently.
    RenderingTasks  renderingTasks_ ;
    CollectingTasks collectingTasks_ ;
    CompositingTasks compositingTasks_ ;
    MakePixmapTasks makePixmapTasks_ ;
    TaskMakePixmap *collagePixmapTask_;

    //Volume Data
    Volume<uchar> *baseVolume_;
    std::vector< Volume<uchar>* > bricks_;

    //Transformation parameters
    Coordinates3D rotation_;
    Coordinates3D translation_;
    float brightness_;
    float volumeDensity_;

    //shared data for multithreads, must not be modified during
    //the activity rendering threads.
    //modified using syncTransformation_()
    Coordinates3D rotationAsync_;
    Coordinates3D translationAsync_;
    float brightnessAsync_;
    float volumeDensityAsync_;

    //flags
    bool pendingTransformations_;
    bool renderingNodesReady_;
    bool compositingNodeSpecified_;

    //counters
    uint8_t activeRenderingNodes_;
    uint8_t readyPixmapsCount_;
    uint8_t compositedFramesCount_ ;

    //facts
    uint8_t machineGPUsCount_;
    const uint frameWidth_ ;
    const uint frameHeight_ ;


};

#endif // PARALLELRENDERING_H
