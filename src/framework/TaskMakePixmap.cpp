#include "TaskMakePixmap.h"
#include "ProfilingExterns.h"

clpar::Task::TaskMakePixmap::TaskMakePixmap(  )
{
    frame_ = nullptr ;
    clRenderer_ = nullptr ;
    setAutoDelete( false );
}

void clpar::Task::TaskMakePixmap::setFrame( clData::CLImage2D< float > *frame )
{
    frame_ = frame ;
}

void clpar::Task::TaskMakePixmap::setRenderer(
        Renderer::CLAbstractRenderer *renderer )
{
    clRenderer_ = renderer ;
}

void clpar::Task::TaskMakePixmap::run()
{
    TIC( frameworkProfile.convertToPixmap_TIMER );
    emit this->pixmapReady_SIGNAL( &( frame_->getFramePixmap( )) , clRenderer_ );
    TOC( frameworkProfile.convertToPixmap_TIMER );
}
