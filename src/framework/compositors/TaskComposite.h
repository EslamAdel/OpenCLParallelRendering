#ifndef TASKCOMPOSITE_H
#define TASKCOMPOSITE_H

#include <QRunnable>
#include "CLRenderer.h"
#include "CLAbstractCompositor.h"
#include <QObject>
#include "Profiles.hh"



namespace clparen {
namespace Compositor {
namespace Task {

/**
 * @brief The TaskComposite class
 */
class TaskComposite : public QObject , public QRunnable
{
    Q_OBJECT

public:
    /**
     * @brief TaskComposite
     * @param CLCompositor
     * @param frameIndex
     * @param compositedFramesCount
     */
    TaskComposite( Compositor::CLAbstractCompositor *compositor ,
                   Renderer::CLAbstractRenderer *cLRenderer );

signals:
    /**
     * @brief compositingFinished_SIGNAL
     * Emit a signal when compositing is done.
     */
    void compositingFinished_SIGNAL() ;

protected:
    void run();

private:
    Compositor::CLAbstractCompositor *compositor_ ;
    Renderer::CLAbstractRenderer *renderer_  ;
};


}
}
}
#endif // TASKCOMPOSITE_H
