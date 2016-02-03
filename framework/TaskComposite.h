#ifndef TASKCOMPOSITE_H
#define TASKCOMPOSITE_H

#include <QRunnable>
#include "RenderingNode.h"
#include "CompositingNode.h"
#include <QObject>

class TaskComposite : public QObject , public QRunnable
{
    Q_OBJECT

public:
    TaskComposite( CompositingNode *compositingNode ,const uint frameIndex );


signals:
    void compositingFinished_SIGNAL() ;

protected:
    void run();

private:
    CompositingNode *compositingNode_;

    const uint frameIndex_ ;


};

#endif // TASKCOMPOSITE_H
