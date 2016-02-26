#include "TaskComposite.h"
#include "ProfilingExterns.h"

#include "Logger.h"
TaskComposite::TaskComposite(CLCompositor *compositor ,
                             CLRenderer *renderer)
    : renderer_( renderer )
{
    compositor_ = compositor ;
    setAutoDelete( false );
}


void TaskComposite::run()
{

    TOC( compositingProfile.threadSpawning_TIMER ) ;

    uint8_t compositedFramesCount =
            compositor_->getCompositedFramesCount( );

    if( compositedFramesCount == 0 )
    {
        TIC( compositingProfile.compositing_TIMER );
    }

    TIC( compositingProfile.accumulatingFrame_TIMER );
    compositor_->accumulateFrame_DEVICE( renderer_ );
    TOC( compositingProfile.accumulatingFrame_TIMER );

    if( compositor_->getCompositedFramesCount()
        == compositor_->framesCount() )
    {

        TIC( compositingProfile.loadCollageFromDevice_TIMER );
        compositor_->loadCollageFromDevice();
        TOC( compositingProfile.loadCollageFromDevice_TIMER );

        TOC( compositingProfile.compositing_TIMER );

        emit this->compositingFinished_SIGNAL();

    }
}

