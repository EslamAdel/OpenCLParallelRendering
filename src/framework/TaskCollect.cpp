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



    //upload frame from rendering GPU to HOST.
    TIC( COLLECTING_PROFILE( renderingNode_ ).transferingBuffer_TIMER );

    compositingNode_->collectFrame( renderingNode_->getFrameIndex() ,
                                    renderingNode_->getCommandQueue() ,
                                    *renderingNode_->getCLFrame() ,
                                    CL_TRUE );

    TOC( COLLECTING_PROFILE(renderingNode_).transferingBuffer_TIMER ) ;


    emit this->frameLoadedToDevice_SIGNAL( renderingNode_ );

}

