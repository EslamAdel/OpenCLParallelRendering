#ifndef TASKCOLLECT_H
#define TASKCOLLECT_H

#include <QObject>
#include <QRunnable>
#include "VirtualNode.h"

class TaskCollect : public QObject , public QRunnable
{
    Q_OBJECT
public:
    TaskCollect( const VirtualNode &vNode);

    void run();

private:
    VirtualNode &vNode_;
};

#endif // TASKCOLLECT_H
