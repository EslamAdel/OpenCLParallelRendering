#include "TaskCollect.h"

TaskCollect::TaskCollect( RenderingNode *renderingNode ,
                          CompositingNode *compositingNode,
                          const uint frameIndex ,
                          CollectingProfile &collectingProfile)
    : renderingNode_( renderingNode ) ,
      compositingNode_( compositingNode ) ,
      frameIndex_( frameIndex ) ,
      collectingProfile_( collectingProfile )
{
    setAutoDelete( false );
}

void TaskCollect::run()
{
    collectingProfile_.threadSpawningTime_.stop();


    collectingProfile_.loadingBufferFromDevice_.start();

    renderingNode_->uploadFrameFromDevice( CL_TRUE );

    collectingProfile_.loadingBufferFromDevice_.stop();



    uint* frameData = renderingNode_->getFrameData();



    compositingNode_->setFrameData_HOST( frameIndex_ , frameData );



    collectingProfile_.loadingBufferToDevice_.start();

    compositingNode_->loadFrameDataToDevice( frameIndex_ , CL_TRUE );

    collectingProfile_.loadingBufferToDevice_.stop();



    emit this->frameLoadedToDevice_SIGNAL( renderingNode_ );


}

