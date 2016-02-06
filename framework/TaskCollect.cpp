#include "TaskCollect.h"

TaskCollect::TaskCollect(RenderingNode *renderingNode ,
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
    CollectingProfile &collectingProfile = *collectingProfiles[ renderingNode_ ];

    collectingProfile.threadSpawningTime_.stop();


    collectingProfile.loadingBufferFromDevice_.start();
    renderingNode_->uploadFrameFromDevice( CL_TRUE );
    collectingProfile.loadingBufferFromDevice_.stop();


    uint* frameData = renderingNode_->getFrameData();

    compositingNode_->setFrameData_HOST( frameIndex_ , frameData );


    collectingProfile.loadingBufferToDevice_.start();
    compositingNode_->loadFrameDataToDevice( frameIndex_ , CL_TRUE );
    collectingProfile.loadingBufferToDevice_.stop();

    emit this->frameLoadedToDevice_SIGNAL( renderingNode_ );

}

