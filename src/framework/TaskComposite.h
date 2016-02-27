#ifndef TASKCOMPOSITE_H
#define TASKCOMPOSITE_H

#include <QRunnable>
#include "CLRenderer.h"
#include "CLAbstractCompositor.h"
#include <QObject>
#include "Profiles.hh"

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
    TaskComposite( CLAbstractCompositor *compositor ,
                   CLAbstractRenderer *cLRenderer );

signals:
    /**
     * @brief compositingFinished_SIGNAL
     * Emit a signal when compositing is done.
     */
    void compositingFinished_SIGNAL() ;

protected:
    void run();

private:
    CLAbstractCompositor *compositor_ ;
    CLAbstractRenderer *renderer_  ;
};

#endif // TASKCOMPOSITE_H
