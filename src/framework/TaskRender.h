#ifndef TASKRENDER_H
#define TASKRENDER_H

#include <QRunnable>
#include "CLRenderer.h"
#include "ProfilingExterns.h"

/**
 * @brief The TaskRender class
 * Wrap the instruction of rendering frame.
 */
class TaskRender : public QRunnable
{
public:
    TaskRender( CLRenderer &cLRenderer  );

protected:
    void run();

private:
    CLRenderer &cLRenderer_;


};

#endif // TASKRENDER_H
