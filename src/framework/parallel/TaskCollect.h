#ifndef TASKCOLLECT_H
#define TASKCOLLECT_H

#include <QRunnable>
#include "CLRenderer.h"
#include "CLCompositor.h"
#include <QObject>
#include "Profiles.hh"
#include "ProfilingExterns.h"

namespace clparen {
namespace Task {

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
    TaskCollect( Renderer::CLAbstractRenderer *renderer ,
                 Compositor::CLAbstractCompositor *compositor );

Q_SIGNALS:

    void frameLoadedToDevice_SIGNAL( uint );

protected:
    void run();

private:
    Renderer::CLAbstractRenderer *renderer_;
    Compositor::CLAbstractCompositor *compositor_;

};

}
}

#endif // TASKCOLLECT_H
