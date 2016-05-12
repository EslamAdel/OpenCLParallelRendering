#ifndef TASKRENDER_H
#define TASKRENDER_H

#include <QRunnable>
#include "CLRenderer.h"
#include "ProfilingExterns.h"


namespace clparen {
namespace Task {

/**
 * @brief The TaskRender class
 * Wrap the instruction of rendering frame.
 */
class TaskRender : public QRunnable
{
public:
    TaskRender( Renderer::CLAbstractRenderer &cLRenderer  );

protected:
    void run();

private:
    Renderer::CLAbstractRenderer &cLRenderer_;


};

}
}

#endif // TASKRENDER_H
