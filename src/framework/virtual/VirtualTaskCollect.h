#ifndef VIRTUALTASKCOLLECT_H
#define VIRTUALTASKCOLLECT_H

#include <QObject>
#include <QRunnable>
#include "VirtualRenderingNode.h"
#include "VirtualCompositingNode.h"

class VirtualTaskCollect : public QObject , public QRunnable
{
    Q_OBJECT

public:

    VirtualTaskCollect( VirtualRenderingNode *renderingNode ,
                        VirtualCompositingNode *compositingNode );


signals:

    void frameLoadedToDevice_SIGNAL( VirtualRenderingNode *node );

protected:
    void run();

private:
    VirtualRenderingNode *renderingNode_;
    VirtualCompositingNode *compositingNode_;
    const uint frameIndex_ ;

};

#endif // VIRTUALTASKCOLLECT_H
