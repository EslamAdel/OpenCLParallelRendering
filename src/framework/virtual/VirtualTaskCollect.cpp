#include "VirtualTaskCollect.h"
#include "Logger.h"

VirtualTaskCollect::VirtualTaskCollect( VirtualRenderingNode *renderingNode,
                                        VirtualCompositingNode *compositingNode)
    : renderingNode_( renderingNode ) ,
      compositingNode_( compositingNode )
{
    setAutoDelete( false );
}

void VirtualTaskCollect::run()
{

    LOG_DEBUG("collecting frame %d", renderingNode_->getGPUIndex() );
    compositingNode_->collectFrame( renderingNode_ ,
                                    CL_TRUE );


    emit this->frameLoadedToDevice_SIGNAL( renderingNode_ );
}
