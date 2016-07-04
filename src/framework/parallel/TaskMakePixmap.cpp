#include "TaskMakePixmap.h"
#include "ProfilingExterns.h"
#include "Logger.h"


namespace clparen {
namespace Task {


TaskMakePixmap::TaskMakePixmap(  )
{
    frame_ = nullptr ;
    clRenderer_ = nullptr ;
    setAutoDelete( false );
}

void TaskMakePixmap::setFrame( CLData::CLImage2D< float > *frame )
{
    frame_ = frame ;
}

void TaskMakePixmap::setRenderer(
        Renderer::CLAbstractRenderer *renderer )
{
    clRenderer_ = renderer ;
    frame_ = renderer->getCLFrame().value< CLData::CLImage2D< float > *>();
}

void TaskMakePixmap::run()
{
    TIC( frameworkProfile.convertToPixmap_TIMER );

    if( frame_ == nullptr )
        LOG_ERROR("Null ptr!");

    Q_EMIT this->pixmapReady_SIGNAL( &( frame_->getFramePixmap( )) , clRenderer_ );
    TOC( frameworkProfile.convertToPixmap_TIMER );
}



}
}
