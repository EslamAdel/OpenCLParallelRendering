#include "TaskCollect.h"

TaskCollect::TaskCollect(RenderingNode &renderingNode)
    : renderingNode_( renderingNode )
{
    setAutoDelete( false );
}

void TaskCollect::run()
{
    renderingNode_.uploadBuffer();
    renderingNode_.frameBufferToPixmap();
}

