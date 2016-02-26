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

    VirtualTaskCollect( VirtualCLRenderer *renderer ,
                        VirtualCLCompositor *compositor );


signals:

    void frameLoadedToDevice_SIGNAL( VirtualCLRenderer *renderer );

protected:
    void run();

private:
    VirtualCLRenderer *renderer_;
    VirtualCLCompositor *compositor_;

};

#endif // VIRTUALTASKCOLLECT_H
