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

    //void distributeBaseVolume();

    //void startRendering();

    void applyTransformation();

public slots :
    void renderingTaskFinished_SLOT( RenderingNode *finishedNode );
    void compositingTaskFinished_SLOT();
    void imageUploaded_SLOT( RenderingNode *finishedNode);



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

    QThreadPool rendererPool_  ;
    QThreadPool compositorPool_;
    QThreadPool collectorPool_ ;

    RenderingTasks  renderingTasks_ ;
    CollectingTasks collectingTasks_;


    Volume<uchar> *baseVolume_;


    bool pendingTransformations_;

    /** @brief rotation_
    */
    Coordinates3D rotation_;

     /**
    * @brief translation_
    */
    Coordinates3D translation_;

    /**
     * @brief volumeDensity_
     */
    float volumeDensity_;

    /**
     * @brief imageBrightness_
     */
    float imageBrightness_;


};

#endif // PARALLELRENDERING_H
