#include "TaskMakePixmap.h"
#include "ProfilingExterns.h"

TaskMakePixmap::TaskMakePixmap(  )
{
    frame_ = nullptr ;
    clRenderer_ = nullptr ;
    setAutoDelete( false );
}

void TaskMakePixmap::setFrame( CLFrame<uint> *frame )
{
    frame_ = frame ;
}

void TaskMakePixmap::setRenderer( CLRenderer *renderer )
{
    clRenderer_ = renderer ;
}

void TaskMakePixmap::run()
{
    TIC( frameworkProfile.convertToPixmap_TIMER );
    emit this->pixmapReady_SIGNAL( &( frame_->getFramePixmap( )) , clRenderer_ );
    TOC( frameworkProfile.convertToPixmap_TIMER );
}
