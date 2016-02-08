#include "TaskComposite.h"
#include "ProfilingExterns.h"


TaskComposite::TaskComposite( CompositingNode *compositingNode )
{
    compositingNode_ = compositingNode ;
    setAutoDelete( false );
}


void TaskComposite::run()
{

    TOC( compositingProfile.threadSpawning_TIMER ) ;

    uint8_t &compositedFramesCount =
            compositingNode_->getCompositedFramesCount( );

    if( compositedFramesCount == 0 )
        TIC( compositingProfile.compositing_TIMER );


    TIC( compositingProfile.accumulatingFrame_TIMER );
    compositingNode_->accumulateFrame_DEVICE( );
    TOC( compositingProfile.accumulatingFrame_TIMER );

    if( ++compositedFramesCount == compositingNode_->framesCount() )
    {
        compositedFramesCount = 0 ;

        TIC( compositingProfile.loadCollageFromDevice_TIMER );
        compositingNode_->uploadCollageFromDevice();
        TOC( compositingProfile.loadCollageFromDevice_TIMER );

        TOC( compositingProfile.compositing_TIMER );

        emit this->compositingFinished_SIGNAL();

    }
}

