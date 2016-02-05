#ifndef TASKRENDER_H
#define TASKRENDER_H

#include <QRunnable>
#include "RenderingNode.h"
#include "Timer.h"

/**
 * @brief The TaskRender class
 * Wrap the instruction of rendering frame.
 */
class TaskRender : public QRunnable
{
public:
    TaskRender( RenderingNode &renderingNode ,
                RenderingProfile &renderingProfile );

protected:
    void run();

private:
    RenderingNode &renderingNode_;
    RenderingProfile &renderingProfile_;


};

#endif // TASKRENDER_H
