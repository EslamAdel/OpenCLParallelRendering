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

    compositingProfile.threadSpawningTime_.stop();

    if( compositedFramesCount_ == 0 )
        compositingProfile.compositingTime_.start();

    compositingProfile.accumulatingFrameTime_.start();
    compositingNode_->accumulateFrame_DEVICE( frameIndex_ );
    compositingProfile.accumulatingFrameTime_.stop();

    if( ++compositedFramesCount_ == compositingNode_->framesCount() )
    {
        compositedFramesCount_ = 0 ;

        compositingProfile.loadCollageFromDeviceTime_.start();
        compositingNode_->uploadCollageFromDevice();
        compositingProfile.loadCollageFromDeviceTime_.stop();

        compositingProfile.rewindCollageTime_.start();
        compositingNode_->rewindCollageFrame_DEVICE( CL_TRUE );
        compositingProfile.rewindCollageTime_.stop();

        compositingProfile.compositingTime_.stop();

        emit this->compositingFinished_SIGNAL();

    }
}

