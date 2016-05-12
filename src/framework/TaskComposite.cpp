#include "TaskComposite.h"
#include "ProfilingExterns.h"

#include "Logger.h"
clpar::Task::TaskComposite::TaskComposite(
        Compositor::CLAbstractCompositor *compositor ,
        Renderer::CLAbstractRenderer *renderer )
    : renderer_( renderer )
{
    compositor_ = compositor ;
    setAutoDelete( false );
}


void clpar::Task::TaskComposite::run()
{
    TOC( compositingProfile.threadSpawning_TIMER ) ;

    compositor_->composite( );

    if( compositor_->readOutReady( ))
    {
        TIC( compositingProfile.loadFinalFromDevice_TIMER );
        compositor_->loadFinalFrame( );
        TOC( compositingProfile.loadFinalFromDevice_TIMER );
        emit this->compositingFinished_SIGNAL( );
    }
}

