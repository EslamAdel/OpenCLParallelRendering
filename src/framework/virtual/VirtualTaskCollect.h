#ifndef VIRTUALTASKCOLLECT_H
#define VIRTUALTASKCOLLECT_H

#include <QObject>
#include <QRunnable>
#include "VirtualCLRenderer.h"
#include "VirtualCLCompositor.h"


namespace clpar {
namespace Task {

class VirtualTaskCollect : public QObject , public QRunnable
{
    Q_OBJECT

public:

    VirtualTaskCollect( Renderer::CLAbstractRenderer *renderer ,
                        Compositor::CLAbstractCompositor *compositor );


signals:

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
