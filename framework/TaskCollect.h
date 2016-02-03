#ifndef TASKCOLLECT_H
#define TASKCOLLECT_H

#include <QRunnable>
#include "RenderingNode.h"
#include "CompositingNode.h"
#include <QObject>

class TaskCollect : public QObject , public QRunnable
{
    Q_OBJECT

public:
    TaskCollect( RenderingNode *renderingNode ,
                 CompositingNode *compositingNode ,
                 const uint frameIndex );

signals:
    void frameLoadedToDevice_SIGNAL( RenderingNode *node );

protected:
    void run();

private:
    RenderingNode *renderingNode_;
    CompositingNode *compositingNode_;
    const uint frameIndex_;
};

#endif // TASKCOLLECT_H
