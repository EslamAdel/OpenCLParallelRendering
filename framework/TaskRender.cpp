#include "TaskRender.h"

TaskRender::TaskRender( RenderingNode &renderingNode )
    : renderingNode_( renderingNode )
{
    setAutoDelete( false );

}

void TaskRender::run()
{

    renderingProfiles[ &renderingNode_ ]->threadSpawningTime_.stop();
    renderingNode_.applyTransformation();

}

