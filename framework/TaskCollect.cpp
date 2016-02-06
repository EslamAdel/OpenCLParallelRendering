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

    TOC( COLLECTING_PROFILE(renderingNode_).threadSpawning_TIMER );

    CLFrame32 *sourceFrame = renderingNode_->getCLFrame();
    TIC( COLLECTING_PROFILE(renderingNode_).loadingBufferFromDevice_TIMER );
    //upload frame from rendering GPU to HOST.
    sourceFrame->readDeviceData( renderingNode_->getCommandQueue() ,
                                 CL_TRUE );
    TOC( COLLECTING_PROFILE(renderingNode_).loadingBufferFromDevice_TIMER );

    //now sourceFrame points to recently uploaded data.
    compositingNode_->setFrameData_HOST( frameIndex_ ,
                                         sourceFrame->getHostData( ));


    TIC( COLLECTING_PROFILE(renderingNode_).loadingBufferToDevice_TIMER ) ;
    compositingNode_->loadFrameDataToDevice( frameIndex_ , CL_TRUE );
    TOC( COLLECTING_PROFILE(renderingNode_).loadingBufferToDevice_TIMER ) ;

    emit this->frameLoadedToDevice_SIGNAL( renderingNode_ );

}

