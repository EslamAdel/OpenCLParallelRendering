#include "TaskRender.h"

clparen::Task::TaskRender::TaskRender( Renderer::CLAbstractRenderer &cLRenderer )
    : cLRenderer_( cLRenderer )
{
    setAutoDelete( false );

}

void clparen::Task::TaskRender::run()
{

    TOC( renderingProfiles[ &cLRenderer_ ]->threadSpawning_TIMER );
    cLRenderer_.applyTransformation();
}

