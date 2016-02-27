#include "TaskComposite.h"
#include "ProfilingExterns.h"

#include "Logger.h"
TaskComposite::TaskComposite( CLAbstractCompositor *compositor ,
                              CLAbstractRenderer *renderer)
    : renderer_( renderer )
{
    compositor_ = compositor ;
    setAutoDelete( false );
}


void TaskComposite::run()
{
    TOC( compositingProfile.threadSpawning_TIMER ) ;

    compositor_->composite();

    if( compositor_->readOutReady( ))
    {
        compositor_->loadFinalFrame();
        emit this->compositingFinished_SIGNAL();
    }
}

