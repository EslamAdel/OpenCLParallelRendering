#include "TaskRender.h"

TaskRender::TaskRender( RenderingNode &renderingNode )
    : renderingNode_( renderingNode )
{
    setAutoDelete( false );

}

void TaskRender::run()
{

//    renderingProfiles[ &renderingNode_ ]->threadSpawning_TIMER.stop();
    TOC( renderingProfiles[ &renderingNode_ ]->threadSpawning_TIMER );
    renderingNode_.applyTransformation();

}

