#ifndef TASKMAKEPIXMAP_H
#define TASKMAKEPIXMAP_H

#include <QObject>
#include <QRunnable>
#include <CLFrame.h>
#include "CLRenderer.h"
#include <CLImage2D.h>

class TaskMakePixmap : public QObject , public QRunnable
{
    Q_OBJECT


public:
    TaskMakePixmap( );

    void setFrame( CLImage2D< float > *frame );
    void setRenderer( CLAbstractRenderer *renderer );


protected:
    void run() ;

signals:
    void pixmapReady_SIGNAL(  QPixmap *pixmap , const CLAbstractRenderer* clRenderer);

private:
    CLImage2D< float > *frame_ ;
    CLAbstractRenderer *clRenderer_ ;

};

#endif // TASKMAKEPIXMAP_H
