#ifndef TASKCOMPOSITE_H
#define TASKCOMPOSITE_H

#include <QRunnable>
#include "RenderingNode.h"
#include "CompositingNode.h"
#include <QObject>
#include "Profiles.hh"

/**
 * @brief The TaskComposite class
 */
class TaskComposite : public QObject , public QRunnable
{
    Q_OBJECT

public:
    /**
     * @brief TaskComposite
     * @param compositingNode
     * @param frameIndex
     * @param compositedFramesCount
     */
    TaskComposite( CompositingNode *compositingNode ,const uint frameIndex ,
                   uint8_t &compositedFramesCount  );

    void patchCompositing() ;

    void accumulateCompositing() ;

signals:
    /**
     * @brief compositingFinished_SIGNAL
     * Emit a signal when compositing is done.
     */
    void compositingFinished_SIGNAL() ;

protected:
    void run();

private:
    CompositingNode *compositingNode_;

    const CompositingNode::CompositingMode mode_ ;

    const uint frameIndex_ ;

    uint8_t &compositedFramesCount_ ;

};

#endif // TASKCOMPOSITE_H
