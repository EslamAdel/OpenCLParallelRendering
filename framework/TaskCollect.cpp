#include "TaskCollect.h"

TaskCollect::TaskCollect( RenderingNode *renderingNode ,
                          CompositingNode *compositingNode,
                          const uint frameIndex )
    : renderingNode_( renderingNode ) ,
      compositingNode_( compositingNode ) ,
      frameIndex_( frameIndex )
{
    setAutoDelete( false );
}

void TaskCollect::run()
{
    renderingNode_->uploadFrameFromDevice( CL_TRUE );

    uint* frameData = renderingNode_->getFrameData();


    compositingNode_->setFrameData_HOST( frameIndex_ , frameData );
    compositingNode_->loadFrameDataToDevice( frameIndex_ , CL_TRUE );

    emit this->frameLoadedToDevice_SIGNAL( renderingNode_ );


}

