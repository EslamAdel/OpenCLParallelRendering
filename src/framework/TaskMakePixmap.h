#ifndef TASKMAKEPIXMAP_H
#define TASKMAKEPIXMAP_H

#include <QObject>
#include <QRunnable>
#include <CLFrame.h>
#include "RenderingNode.h"

class TaskMakePixmap : public QObject , public QRunnable
{
    Q_OBJECT
public:
    TaskMakePixmap( CLFrame32 *&frame , const RenderingNode* node = nullptr );

protected:
    void run() ;

signals:
    void pixmapReady_SIGNAL(  QPixmap *pixmap , const RenderingNode* node);

private:
    CLFrame32 *&frame_ ;
    const RenderingNode *node_;

};

#endif // TASKMAKEPIXMAP_H
