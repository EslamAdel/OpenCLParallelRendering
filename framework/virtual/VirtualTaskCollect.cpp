#include "VirtualTaskCollect.h"
#include "Logger.h"

VirtualTaskCollect::VirtualTaskCollect( VirtualRenderingNode *renderingNode,
                                        VirtualCompositingNode *compositingNode)
    : renderingNode_( renderingNode ) ,
      compositingNode_( compositingNode ) ,
      frameIndex_( renderingNode_->getFrameIndex() )
{
    setAutoDelete( false );
}

void VirtualTaskCollect::run()
{

    LOG_DEBUG("collecting frame %d", frameIndex_ );
    compositingNode_->collectFrame( frameIndex_ ,
                                    renderingNode_->getCommandQueue() ,
                                    *renderingNode_->getCLFrame() ,
                                    CL_TRUE );


    emit this->frameLoadedToDevice_SIGNAL( renderingNode_ );
}
