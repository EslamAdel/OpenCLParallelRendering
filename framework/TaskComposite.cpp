#include "TaskComposite.h"

TaskComposite::TaskComposite( CompositingNode *compositingNode ,
                              const uint frameIndex ,
                              uint8_t &compositedFramesCount )
    : frameIndex_( frameIndex ),
      compositedFramesCount_( compositedFramesCount )
{
    compositingNode_ = compositingNode ;
    setAutoDelete( false );
}

void TaskComposite::run()
{
//    static int framesLoaded = 0 ;
//    if( ++framesLoaded == compositingNode_->framesCount() )
//    {
//        framesLoaded = 0 ;
//        compositingNode_->compositeFrames_DEVICE();
//        compositingNode_->uploadCollageFromDevice();
//        compositingNode_->rewindCollageFrame_DEVICE( CL_TRUE );
//        emit this->compositingFinished_SIGNAL();

//    }

    compositingNode_->accumulateFrame_DEVICE( frameIndex_ );

    if( ++compositedFramesCount_ == compositingNode_->framesCount() )
    {
        compositedFramesCount_ = 0 ;
        compositingNode_->uploadCollageFromDevice();
        compositingNode_->rewindCollageFrame_DEVICE( CL_TRUE );
        emit this->compositingFinished_SIGNAL();

    }

}

