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



    CLFrame32 *sourceFrame = renderingNode_->getCLFrame();


    collectingProfile.loadingBufferFromDevice_.start();
    //upload frame from rendering GPU to HOST.
    sourceFrame->readDeviceData( renderingNode_->getCommandQueue() ,
                                 CL_TRUE );
    collectingProfile.loadingBufferFromDevice_.stop();

    //now sourceFrame points to recently uploaded data.
    compositingNode_->setFrameData_HOST( frameIndex_ ,
                                         sourceFrame->getHostData( ));


    collectingProfile.loadingBufferToDevice_.start();
    compositingNode_->loadFrameDataToDevice( frameIndex_ , CL_TRUE );
    collectingProfile.loadingBufferToDevice_.stop();

    emit this->frameLoadedToDevice_SIGNAL( renderingNode_ );

}

