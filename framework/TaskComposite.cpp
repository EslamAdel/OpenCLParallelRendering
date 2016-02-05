#include "TaskComposite.h"

TaskComposite::TaskComposite( CompositingNode *compositingNode ,
                              const uint frameIndex ,
                              uint8_t &compositedFramesCount ,
                              CompositingProfile &compositingProfile)
    : frameIndex_( frameIndex ),
      compositedFramesCount_( compositedFramesCount ) ,
      mode_( compositingNode->compositingMode() ),
      compositingProfile_( compositingProfile )
{
    compositingNode_ = compositingNode ;
    setAutoDelete( false );
}

//void TaskComposite::patchCompositing()
//{
//    static int framesLoaded = 0 ;
//    if( ++framesLoaded == compositingNode_->framesCount() )
//    {
//        framesLoaded = 0 ;
//        compositingNode_->compositeFrames_DEVICE();
//        compositingNode_->uploadCollageFromDevice();
//        compositingNode_->rewindCollageFrame_DEVICE( CL_TRUE );
//        emit this->compositingFinished_SIGNAL();

//    }
//}

//void TaskComposite::accumulateCompositing()
//{
//    compositingNode_->accumulateFrame_DEVICE( frameIndex_ );

//    if( ++compositedFramesCount_ == compositingNode_->framesCount() )
//    {
//        compositedFramesCount_ = 0 ;
//        compositingNode_->uploadCollageFromDevice();
//        compositingNode_->rewindCollageFrame_DEVICE( CL_TRUE );
//        emit this->compositingFinished_SIGNAL();

//    }
//}


//void TaskComposite::run()
//{
    //    if( mode_ == CompositingNode::CompositingMode::Accumulate )
    //        accumulateCompositing();
    //    else
    //        patchCompositing();

//}



void TaskComposite::run()
{
    compositingProfile_.threadSpawningTime_.stop();

    if( compositedFramesCount_ == 0 )
        compositingProfile_.compositingTime_.start();



    compositingProfile_.accumulatingFrameTime_.start();
    compositingNode_->accumulateFrame_DEVICE( frameIndex_ );
    compositingProfile_.accumulatingFrameTime_.stop();

    if( ++compositedFramesCount_ == compositingNode_->framesCount() )
    {
        compositedFramesCount_ = 0 ;

        compositingProfile_.loadCollageFromDeviceTime_.start();
        compositingNode_->uploadCollageFromDevice();
        compositingProfile_.loadCollageFromDeviceTime_.stop();

        compositingProfile_.rewindCollageTime_.start();
        compositingNode_->rewindCollageFrame_DEVICE( CL_TRUE );
        compositingProfile_.rewindCollageTime_.stop();

        emit this->compositingFinished_SIGNAL();

        compositingProfile_.compositingTime_.stop();
    }
}

