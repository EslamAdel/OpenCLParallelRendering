#ifndef VIRTUALTASKCOLLECT_H
#define VIRTUALTASKCOLLECT_H

#include <QObject>
#include <QRunnable>
#include "VirtualCLRenderer.h"
#include "VirtualCLCompositor.h"


namespace clparen {
namespace Task {

class VirtualTaskCollect : public QObject , public QRunnable
{
    Q_OBJECT

public:

    VirtualTaskCollect( Renderer::CLAbstractRenderer *renderer ,
                        Compositor::CLAbstractCompositor *compositor );


Q_SIGNALS:

    void frameLoadedToDevice_SIGNAL(
            Renderer::CLAbstractRenderer *renderer );

protected:
    void run();

private:
    Renderer::CLAbstractRenderer *renderer_;
    Compositor::CLAbstractCompositor *compositor_;

};


}
}

#endif // VIRTUALTASKCOLLECT_H
