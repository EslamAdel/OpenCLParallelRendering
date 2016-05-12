#include "TaskMakePixmap.h"
#include "ProfilingExterns.h"

clparen::Task::TaskMakePixmap::TaskMakePixmap(  )
{
    frame_ = nullptr ;
    clRenderer_ = nullptr ;
    setAutoDelete( false );
}

void clparen::Task::TaskMakePixmap::setFrame( clData::CLImage2D< float > *frame )
{
    frame_ = frame ;
}

void clparen::Task::TaskMakePixmap::setRenderer(
        Renderer::CLAbstractRenderer *renderer )
{
    clRenderer_ = renderer ;
}

void clparen::Task::TaskMakePixmap::run()
{
    TIC( frameworkProfile.convertToPixmap_TIMER );
    emit this->pixmapReady_SIGNAL( &( frame_->getFramePixmap( )) , clRenderer_ );
    TOC( frameworkProfile.convertToPixmap_TIMER );
}
