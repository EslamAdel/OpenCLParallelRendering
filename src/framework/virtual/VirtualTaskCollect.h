#ifndef VIRTUALTASKCOLLECT_H
#define VIRTUALTASKCOLLECT_H

#include <QObject>
#include <QRunnable>
#include "VirtualCLRenderer.h"
#include "VirtualCLCompositor.h"

class VirtualTaskCollect : public QObject , public QRunnable
{
    Q_OBJECT

public:

    VirtualTaskCollect( CLAbstractRenderer *renderer ,
                        CLAbstractCompositor *compositor );


signals:

    void frameLoadedToDevice_SIGNAL(
            CLAbstractRenderer *renderer );

protected:
    void run();

private:
    CLAbstractRenderer *renderer_;
    CLAbstractCompositor *compositor_;

};

#endif // VIRTUALTASKCOLLECT_H
