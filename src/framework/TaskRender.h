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
    TaskRender( CLAbstractRenderer &cLRenderer  );

protected:
    void run();

private:
    CLAbstractRenderer &cLRenderer_;


};

#endif // TASKRENDER_H
