#include "TaskCollect.h"
#include "Logger.h"


namespace clparen {
namespace Task {


TaskCollect::TaskCollect(
        Renderer::CLAbstractRenderer *renderer ,
        Compositor::CLAbstractCompositor *compositor )
    : renderer_( renderer ) ,
      compositor_( compositor )
{
    setAutoDelete( false );
}

void TaskCollect::run()
{


}




}
}
