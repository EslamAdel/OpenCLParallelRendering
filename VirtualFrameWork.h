#ifndef VIRTUALFRAMEWORK_H
#define VIRTUALFRAMEWORK_H
#include <QObject>
#include <QList>
#include <QThreadPool>
#include <unordered_map>
#include "VirtualNode.h"
#include "VirtualVolume.h"
#include "VirtualGPU.h"
#include "TaskRender.h"
#include "TaskCollect.h"
#include "TaskComposite.h"
#include "Transformation.h"

#define DEMO


/**
 * @brief NodesRenderTasks
 * For each node, there will be specific QRunnable object created before starting the rendering loop.
 * Although using the same QRunnable object more than one time is not advised, allocating the object
 * before the rendering loop will avoid the dynamic allocation overhead imposed by allocating QRunnable
 * object for every new frame.
 */
typedef std::unordered_map<const VirtualNode*, TaskRender*> NodesRenderTasks;
typedef std::unordered_map<const VirtualNode*, TaskCollect*> NodesCollectTasks;

class VirtualFrameWork : public QObject
{
    Q_OBJECT
public :

enum FrameWorkMode{ AutoTest } ;

    VirtualFrameWork( VirtualVolume &volume ,
                      const TaskComposite::CompositingMode compositingMode = TaskComposite::CompositingMode::AllOnce ,
                      const FrameWorkMode frameWorkMode = FrameWorkMode::AutoTest );

    void addVirtualNode() ;

    void startRendering(const Transformation *transformation = nullptr );
    void applyTramsformation(const Transformation *transformation);


    Transformation &globalTransformation();

    bool transformationsBlocked() const;
private:
    void flushScreen_();
    void distributeVolume_();

public slots :
    void slotNodeFinishedRendering( VirtualNode *vNode );
    void slotNodeFinishedCompositing();
    void slotNodeImageUploaded( VirtualNode *vNode);
    void slotNewTransformations();

#ifdef DEMO
    void slotApplyNewTransformation(bool);
#endif

signals :
    void blockTransformations(bool);
    void blockNewNodes(bool);

private:
    VirtualVolume &mainVolume_;
    bool volumeDistributed_;
    const FrameWorkMode frameWorkmode_;
    TaskComposite *taskComposite_;

    QList<VirtualNode*> nodes_;
    NodesRenderTasks nodesRenderTasks_;
    NodesCollectTasks nodesCollectTasks_;
    bool blockNewNodes_;

    Transformation globalTransformation_;
    bool transformationsBlocked_ ;
    VirtualGPU serverGPU_;

    QThreadPool renderer_ ; // producer for collector
    QThreadPool collector_; // consumer for renderer, producer for compositor
    QThreadPool compositor_; // consumer for collector

};

#endif // VIRTUALFRAMEWORK_H
