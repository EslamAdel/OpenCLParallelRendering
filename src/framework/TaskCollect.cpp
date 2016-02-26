#include "TaskCollect.h"
#include "Logger.h"

TaskCollect::TaskCollect( CLRenderer *renderer ,
                          CLAbstractCompositor *compositor )
    : renderer_( renderer ) ,
      compositor_( compositor )
{
    setAutoDelete( false );
}

void TaskCollect::run()
{

    TOC( COLLECTING_PROFILE(renderer_).threadSpawning_TIMER );



    //upload frame from rendering GPU to HOST.
    TIC( COLLECTING_PROFILE( renderer_ ).transferingBuffer_TIMER );

    compositor_->collectFrame( renderer_ ,
                               CL_TRUE );

    TOC( COLLECTING_PROFILE(renderer_).transferingBuffer_TIMER ) ;


    emit this->frameLoadedToDevice_SIGNAL( renderer_ );

}

