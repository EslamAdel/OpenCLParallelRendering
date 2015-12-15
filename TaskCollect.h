#ifndef TASKCOLLECT_H
#define TASKCOLLECT_H

#include <QRunnable>

class TaskCollect : public QObject , public QRunnable
{
public:
    TaskCollect();
};

#endif // TASKCOLLECT_H
