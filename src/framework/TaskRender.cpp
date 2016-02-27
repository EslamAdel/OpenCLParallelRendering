#include "TaskRender.h"

TaskRender::TaskRender( CLAbstractRenderer &cLRenderer )
    : cLRenderer_( cLRenderer )
{
    setAutoDelete( false );

}

void TaskRender::run()
{

    TOC( renderingProfiles[ &cLRenderer_ ]->threadSpawning_TIMER );
    cLRenderer_.applyTransformation();
}

