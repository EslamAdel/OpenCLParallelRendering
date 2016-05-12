#ifndef TASKMAKEPIXMAP_H
#define TASKMAKEPIXMAP_H

#include <QObject>
#include <QRunnable>
#include <CLFrame.h>
#include "CLRenderer.h"
#include <CLImage2D.h>



namespace clparen { namespace Task {

class TaskMakePixmap : public QObject , public QRunnable
{
    Q_OBJECT


public:
    TaskMakePixmap( );

    void setFrame( clData::CLImage2D< float > *frame );
    void setRenderer( Renderer::CLAbstractRenderer *renderer );


protected:
    void run() ;

signals:
    void pixmapReady_SIGNAL(  QPixmap *pixmap ,
                              const clparen::Renderer::CLAbstractRenderer* clRenderer);

private:
    clData::CLImage2D< float > *frame_ ;
    Renderer::CLAbstractRenderer *clRenderer_ ;

};


}}

#endif // TASKMAKEPIXMAP_H
