#include "TaskComposite.h"
#include "ProfilingExterns.h"

#include "Logger.h"
TaskComposite::TaskComposite(CompositingNode *compositingNode ,
                             RenderingNode *renderingNode)
    : renderingNode_( renderingNode )
{
    compositingNode_ = compositingNode ;
    setAutoDelete( false );
}


void TaskComposite::run()
{

    TOC( compositingProfile.threadSpawning_TIMER ) ;

    uint8_t compositedFramesCount =
            compositingNode_->getCompositedFramesCount( );

    if( compositedFramesCount == 0 )
    {
        TIC( compositingProfile.compositing_TIMER );
    }

    TIC( compositingProfile.accumulatingFrame_TIMER );
    compositingNode_->accumulateFrame_DEVICE( renderingNode_ );
    TOC( compositingProfile.accumulatingFrame_TIMER );

    if( compositingNode_->getCompositedFramesCount()
        == compositingNode_->framesCount() )
    {

        TIC( compositingProfile.loadCollageFromDevice_TIMER );
        compositingNode_->loadCollageFromDevice();
        TOC( compositingProfile.loadCollageFromDevice_TIMER );

        TOC( compositingProfile.compositing_TIMER );

        emit this->compositingFinished_SIGNAL();

    }
}

