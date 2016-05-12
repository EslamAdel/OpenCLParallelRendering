#include "VirtualTaskCollect.h"
#include "Logger.h"

clpar::Task::VirtualTaskCollect::VirtualTaskCollect(
        Renderer::CLAbstractRenderer *renderer,
        Compositor::CLAbstractCompositor *compositor )
    : renderer_( renderer ) ,
      compositor_( compositor )
{
    setAutoDelete( false );
}

void clpar::Task::VirtualTaskCollect::run()
{

    LOG_DEBUG("collecting frame %d", renderer_->getGPUIndex() );
    compositor_->collectFrame( renderer_ ,
                               CL_TRUE );


    emit this->frameLoadedToDevice_SIGNAL( renderer_ );
}
