#ifndef TASKRENDER_H
#define TASKRENDER_H

#include <QRunnable>
#include "RenderingNode.h"
#include "ProfilingExterns.h"

/**
 * @brief The TaskRender class
 * Wrap the instruction of rendering frame.
 */
class TaskRender : public QRunnable
{
public:
    TaskRender( RenderingNode &renderingNode  );

protected:
    void run();

private:
    RenderingNode &renderingNode_;


};

#endif // TASKRENDER_H
