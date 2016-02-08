#include "TaskComposite.h"
#include "ProfilingExterns.h"


TaskComposite::TaskComposite(CompositingNode *compositingNode ,
                              const uint frameIndex ,
                              uint8_t &compositedFramesCount )
    : frameIndex_( frameIndex ),
      compositedFramesCount_( compositedFramesCount ) ,
      mode_( compositingNode->compositingMode() )
{
    compositingNode_ = compositingNode ;
    setAutoDelete( false );
}


void TaskComposite::run()
{

    TOC( compositingProfile.threadSpawning_TIMER ) ;

    if( compositedFramesCount_ == 0 )
        TIC( compositingProfile.compositing_TIMER );


    TIC( compositingProfile.accumulatingFrame_TIMER );
    compositingNode_->accumulateFrame_DEVICE( frameIndex_ );
    TOC( compositingProfile.accumulatingFrame_TIMER );

    if( ++compositedFramesCount_ == compositingNode_->framesCount() )
    {
        compositedFramesCount_ = 0 ;

        TIC( compositingProfile.loadCollageFromDevice_TIMER );
        compositingNode_->uploadCollageFromDevice();
        TOC( compositingProfile.loadCollageFromDevice_TIMER );

        TIC( compositingProfile.rewindCollage_TIMER );
        compositingNode_->rewindCollageFrame_DEVICE( CL_TRUE );
        TOC( compositingProfile.rewindCollage_TIMER );

        TOC( compositingProfile.compositing_TIMER );

        emit this->compositingFinished_SIGNAL();

    }
}

