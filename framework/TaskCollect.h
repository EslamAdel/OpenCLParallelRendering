#ifndef TASKCOLLECT_H
#define TASKCOLLECT_H

#include <QRunnable>
#include "RenderingNode.h"

class TaskCollect : public QRunnable
{
public:
    TaskCollect( RenderingNode &renderingNode );

protected:
    void run();

private:
    RenderingNode &renderingNode_;

};

#endif // TASKCOLLECT_H
