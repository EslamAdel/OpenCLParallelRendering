#ifndef TASKCOLLECT_H
#define TASKCOLLECT_H

#include <QRunnable>
#include "RenderingNode.h"
#include "CompositingNode.h"
#include <QObject>
#include "Profiles.hh"
#include "ProfilingExterns.h"



/**
 * @brief The TaskCollect class
 * This object wraps instructions to transfer rendered buffers
 * from rendering GPU and load it into Compositing devices.
 */
class TaskCollect : public QObject , public QRunnable
{
    Q_OBJECT

public:
    /**
     * @brief TaskCollect
     * @param renderingNode
     * RenderingNode where rendering is finished and buffers
     * need to be uploaded from rendering GPU.
     * @param compositingNode
     * CompositingNode where the buffers will be loaded into.
     * @param frameIndex
     * The index of the rendered frame.
     */
    TaskCollect( RenderingNode *renderingNode ,
                 CompositingNode *compositingNode );

signals:
    /**
     * @brief frameLoadedToDevice_SIGNAL
     * Inform the outside world the task is done.
     * @param node
     * pass the RenderingNode pointer as Identifier for: the
     * RenderingNode and the CompositingNode and the frameIndex_ .
     * Considering each RenderingNode is mapped to a CompositingNode.
     */
    void frameLoadedToDevice_SIGNAL( RenderingNode *node );

protected:
    void run();

private:
    RenderingNode *renderingNode_;
    CompositingNode *compositingNode_;

};

#endif // TASKCOLLECT_H
