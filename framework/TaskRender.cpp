#include "TaskRender.h"

TaskRender::TaskRender( RenderingNode &renderingNode,
                        RenderingProfile &renderingProfile)
    : renderingNode_( renderingNode ),
      renderingProfile_( renderingProfile )
{
    setAutoDelete( false );

}

void TaskRender::run()
{
    renderingProfile_.threadSpawningTime_.stop();

    renderingNode_.applyTransformation();

}

