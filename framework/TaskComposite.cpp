#include "TaskComposite.h"
#include "ProfilingExterns.h"

#include "Logger.h"
TaskComposite::TaskComposite( CompositingNode *compositingNode ,
                             uint frameIndex)
    : frameIndex_( frameIndex )
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
        //LOG_DEBUG("GPU<%d> rendered buffer as collage buffer",co)
    }

    TIC( compositingProfile.accumulatingFrame_TIMER );
    compositingNode_->accumulateFrame_DEVICE( frameIndex_ );
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

