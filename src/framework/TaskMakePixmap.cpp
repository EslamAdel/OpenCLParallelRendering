#include "TaskMakePixmap.h"
#include "ProfilingExterns.h"

TaskMakePixmap::TaskMakePixmap(  )
{
    frame_ = nullptr ;
    clRenderer_ = nullptr ;
    setAutoDelete( false );
}

void TaskMakePixmap::setFrame( CLImage2D< float > *frame )
{
    frame_ = frame ;
}

void TaskMakePixmap::setRenderer( CLAbstractRenderer *renderer )
{
    clRenderer_ = renderer ;
}

void TaskMakePixmap::run()
{
    TIC( frameworkProfile.convertToPixmap_TIMER );
    emit this->pixmapReady_SIGNAL( &( frame_->getFramePixmap( )) , clRenderer_ );
    TOC( frameworkProfile.convertToPixmap_TIMER );
}
