#include "TaskMakePixmap.h"
#include "ProfilingExterns.h"

TaskMakePixmap::TaskMakePixmap( CLFrame32 *&frame ,
                                 const RenderingNode *node )
    : frame_( frame ) ,
      node_( node )
{
    setAutoDelete( false );

}


void TaskMakePixmap::run()
{

    emit this->pixmapReady_SIGNAL( &frame_->getFramePixmap() , node_ );

}
