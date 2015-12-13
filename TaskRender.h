#ifndef TASKRENDER_H
#define TASKRENDER_H

#include <QThreadPool>

class TaskRender : public QObject , public QThreadPool
{
public:
    TaskRender();
};

#endif // TASKRENDER_H
