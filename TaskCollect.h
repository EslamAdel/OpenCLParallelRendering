#ifndef TASKCOLLECT_H
#define TASKCOLLECT_H

#include <QThreadPool>

class TaskCollect : public QObject , public QThreadPool
{
public:
    TaskCollect();
};

#endif // TASKCOLLECT_H
