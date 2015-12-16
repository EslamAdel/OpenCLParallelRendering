#include "TaskCollect.h"

TaskCollect::TaskCollect(const VirtualNode &vNode)
    : vNode_( vNode )
{
    setAutoDelete( false );
}

void TaskCollect::run()
{
    vNode_.uploadImage();
}
