#include "TaskMakePixmap.h"
#include "ProfilingExterns.h"

TaskMakePixmap::TaskMakePixmap(CLFrame32 *&frame ,
                                 const CLRenderer *clRenderer )
    : frame_( frame ) ,
      clRenderer_( clRenderer )
{
    setAutoDelete( false );

}


void TaskMakePixmap::run()
{
    TIC( frameworkProfile.convertToPixmap_TIMER );
    emit this->pixmapReady_SIGNAL( &frame_->getFramePixmap() , clRenderer_ );
    TOC( frameworkProfile.convertToPixmap_TIMER );
}
