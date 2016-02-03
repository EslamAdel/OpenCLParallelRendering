#include "TaskComposite.h"

TaskComposite::TaskComposite( CompositingNode *compositingNode ,
                              const uint frameIndex )
    : frameIndex_( frameIndex )
{
    compositingNode_ = compositingNode ;
    setAutoDelete( false );
}

void TaskComposite::run()
{

    compositingNode_->accumulateFrame_DEVICE( frameIndex_ );

    emit this->compositingFinished_SIGNAL();

}

