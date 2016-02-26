#include "TaskComposite.h"
#include "ProfilingExterns.h"

#include "Logger.h"
TaskComposite::TaskComposite( CLAbstractCompositor *compositor ,
                             CLRenderer *renderer)
    : renderer_( renderer )
{
    compositor_ = compositor ;
    setAutoDelete( false );
    firstTime_ = true ;
}


void TaskComposite::run()
{

    TOC( compositingProfile.threadSpawning_TIMER ) ;

    if( firstTime_ )
    {
        TIC( compositingProfile.compositing_TIMER );
        firstTime_ = false ;
    }

    compositor_->composite();

    if( compositor_->readOutReady( ))
    {
        compositor_->loadFinalFrame();
        emit this->compositingFinished_SIGNAL();
        firstTime_ = true ;
        TOC( compositingProfile.compositing_TIMER );
    }

}

