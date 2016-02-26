#ifndef TASKMAKEPIXMAP_H
#define TASKMAKEPIXMAP_H

#include <QObject>
#include <QRunnable>
#include <CLFrame.h>
#include "CLRenderer.h"

class TaskMakePixmap : public QObject , public QRunnable
{
    Q_OBJECT
public:
    TaskMakePixmap(CLFrame32 *&frame , const CLRenderer* clRenderer = nullptr );

protected:
    void run() ;

signals:
    void pixmapReady_SIGNAL(  QPixmap *pixmap , const CLRenderer* clRenderer);

private:
    CLFrame32 *&frame_ ;
    const CLRenderer *clRenderer_;

};

#endif // TASKMAKEPIXMAP_H
