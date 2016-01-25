#ifndef TASKCOMPOSITE_H
#define TASKCOMPOSITE_H

#include <QRunnable>
#include "RenderingNode.h"

class TaskComposite : public QRunnable
{
public:
    TaskComposite();

protected:
    void run();

private:
    RenderingNode *compositingNode_;



};

#endif // TASKCOMPOSITE_H
