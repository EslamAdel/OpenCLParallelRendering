#include "TaskRender.h"

clpar::Task::TaskRender::TaskRender( Renderer::CLAbstractRenderer &cLRenderer )
    : cLRenderer_( cLRenderer )
{
    setAutoDelete( false );

}

void clpar::Task::TaskRender::run()
{

    TOC( renderingProfiles[ &cLRenderer_ ]->threadSpawning_TIMER );
    cLRenderer_.applyTransformation();
}

