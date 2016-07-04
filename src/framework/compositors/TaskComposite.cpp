#include "TaskComposite.h"
#include "ProfilingExterns.h"

#include "Logger.h"


namespace clparen {
namespace Compositor {
namespace Task {


TaskComposite::TaskComposite(
        Compositor::CLAbstractCompositor *compositor ,
        Renderer::CLAbstractRenderer *renderer )
    : renderer_( renderer )
{
    compositor_ = compositor ;
    setAutoDelete( false );
}


void TaskComposite::run()
{
    TOC( compositingProfile.threadSpawning_TIMER ) ;

    compositor_->composite( );

    if( compositor_->readOutReady( ))
    {
        TIC( compositingProfile.loadFinalFromDevice_TIMER );
        compositor_->loadFinalFrame( );
        TOC( compositingProfile.loadFinalFromDevice_TIMER );
        Q_EMIT this->compositingFinished_SIGNAL( );
    }
}


}
}
}
