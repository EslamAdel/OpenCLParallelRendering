#ifndef PARALLELRENDERING_H
#define PARALLELRENDERING_H


#include <CLContext.h>
#include <Headers.hh>
#include <Volume.h>
#include "RenderingNode.h"
#include <QObject>
#include <QList>
#include <QSet>
#include <QThreadPool>
#include <unordered_map>
#include "TaskRender.h"
#include "TaskCollect.h"
#include "TaskComposite.h"
#include "Transformation.h"
#include "oclHWDL.h"
#include <unordered_map>
#include "Transformation.h"
#include "CompositingNode.h"

typedef std::unordered_map<const oclHWDL::Device*,RenderingNode*> RenderingNodes;
typedef std::unordered_map<const RenderingNode* ,TaskRender*> RenderingTasks;
typedef std::unordered_map<const RenderingNode* ,TaskCollect*> CollectingTasks;

class ParallelRendering : public QObject
{
    Q_OBJECT

public:
    ParallelRendering( Volume< uchar >* volume );

    void loadBaseVolume( Volume<uchar> *volume );

    void discoverAllNodes();

    void addRenderingNode( const uint64_t gpuIndex );

    void addCompositingNode( const uint64_t gpuIndex );

    void distributeBaseVolume1D();

    void startRendering();

    void applyTransformation();

    void syncTransformation();

    RenderingNode &getRenderingNode( const uint64_t gpuIndex );

    uint8_t machineGPUsCount() const;

    uint8_t activeRenderingNodesCount() const;

signals:
    void framesReady_SIGNAL();


public slots :
    void finishedRendering_SLOT(RenderingNode *finishedNode);
    void compositingTaskFinished_SLOT();
    void bufferUploaded_SLOT( RenderingNode *finishedNode);


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

    oclHWDL::Hardware         clHardware_;
    QSet< oclHWDL::Device* >   inUseGPUs_;
    oclHWDL::Devices            listGPUs_;
    RenderingNodes        renderingNodes_;
    CompositingNode     *compositingNode_;

    //threadpools
    QThreadPool rendererPool_  ; //[producer] for collector pool.
    QThreadPool compositorPool_; //[consumer] for collector pool.
    QThreadPool collectorPool_ ; //[producer] for renderer pool AND
                                 //[consumer] for renderer pool.

    RenderingTasks  renderingTasks_ ;
    CollectingTasks collectingTasks_;


    Volume<uchar> *baseVolume_;
    std::vector< Volume<uchar>* > bricks_;
    std::vector< Coordinates3D * > framesCenters_ ;

    Coordinates3D rotation_;
    Coordinates3D translation_;
    float brightness_;
    float volumeDensity_;

    //shared data for multithreads, must not be modified during
    //rendering threads' lives.
    //modified using syncTransformation_()
    Coordinates3D rotationAsync_;
    Coordinates3D translationAsync_;
    float brightnessAsync_;
    float volumeDensityAsync_;

    //flags
    bool pendingTransformations_;
    bool renderingNodesReady_;


    //counters
    uint8_t activeRenderingNodes_;
    uint8_t readyPixmapsCount_;

    //facts
    uint8_t machineGPUsCount_;




};

#endif // PARALLELRENDERING_H
