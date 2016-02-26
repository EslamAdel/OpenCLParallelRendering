#ifndef TASKCOLLECT_H
#define TASKCOLLECT_H

#include <QRunnable>
#include "CLRenderer.h"
#include "CLCompositor.h"
#include <QObject>
#include "Profiles.hh"
#include "ProfilingExterns.h"



/**
 * @brief The TaskCollect class
 * This object wraps instructions to transfer rendered buffers
 * from rendering GPU and load it into Compositing devices.
 */
class TaskCollect : public QObject , public QRunnable
{
    Q_OBJECT

public:
    /**
     * @brief TaskCollect
     * @param CLRenderer
     * CLRenderer where rendering is finished and buffers
     * need to be uploaded from rendering GPU.
     * @param CLCompositor
     * CLCompositor where the buffers will be loaded into.
     * @param frameIndex
     * The index of the rendered frame.
     */
    TaskCollect( CLRenderer *renderer ,
                 CLAbstractCompositor *compositor );

signals:
    /**
     * @brief frameLoadedToDevice_SIGNAL
     * Inform the outside world the task is done.
     * @param node
     * pass the CLRenderer pointer as Identifier for: the
     * CLRenderer and the CLCompositor and the frameIndex_ .
     * Considering each CLRenderer is mapped to a CLCompositor.
     */
    void frameLoadedToDevice_SIGNAL( CLRenderer *renderer );

protected:
    void run();

private:
    CLRenderer *renderer_;
    CLAbstractCompositor *compositor_;

};

#endif // TASKCOLLECT_H
