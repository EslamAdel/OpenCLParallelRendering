#include "TaskRender.h"


namespace clparen {
namespace Renderer {
namespace Task {


TaskRender::TaskRender( Renderer::CLAbstractRenderer &cLRenderer )
    : cLRenderer_( cLRenderer )
{
    setAutoDelete( false );

}

void TaskRender::run()
{

    TOC( renderingProfiles[ &cLRenderer_ ]->threadSpawning_TIMER );
    cLRenderer_.applyTransformation();
}

}
}
}
