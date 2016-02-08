#include "TaskCollect.h"
#include "Logger.h"

TaskCollect::TaskCollect(RenderingNode *renderingNode ,
                         CompositingNode *compositingNode )
    : renderingNode_( renderingNode ) ,
      compositingNode_( compositingNode )
{
    setAutoDelete( false );
}

void TaskCollect::run()
{

    TOC( COLLECTING_PROFILE(renderingNode_).threadSpawning_TIMER );

    CLFrame32 *sourceFrame = renderingNode_->getCLFrame();


    //upload frame from rendering GPU to HOST.
    TIC( COLLECTING_PROFILE( renderingNode_ ).loadingBufferFromDevice_TIMER );
    sourceFrame->readDeviceData( renderingNode_->getCommandQueue() , CL_TRUE );
    TOC( COLLECTING_PROFILE( renderingNode_ ).loadingBufferFromDevice_TIMER );

    //now sourceFrame points to recently uploaded data.
    compositingNode_->setFrameData_HOST( sourceFrame->getHostData( ) ,
                                         renderingNode_->getFrameIndex() );


//    LOG_DEBUG("Writing Rendered frame[%d] from GPU<%d> to compositor" ,
//              renderingNode_->getFrameIndex() ,
//              renderingNode_->getGPUIndex() );

    TIC( COLLECTING_PROFILE(renderingNode_).loadingBufferToDevice_TIMER ) ;
    compositingNode_->loadFrameDataToDevice( renderingNode_->getFrameIndex() ,
                                             CL_TRUE );
    TOC( COLLECTING_PROFILE(renderingNode_).loadingBufferToDevice_TIMER ) ;


//    LOG_DEBUG("[DONE] Writing Rendered frame[%d] from GPU<%d> to compositor" ,
//              renderingNode_->getFrameIndex() ,
//              renderingNode_->getGPUIndex() );

    emit this->frameLoadedToDevice_SIGNAL( renderingNode_ );

}

