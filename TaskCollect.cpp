#include "TaskCollect.h"
#include <iostream>
#include <QThread>
#include "Logger.h"

TaskCollect::TaskCollect( VirtualNode &vNode)
    : vNode_( vNode )
{
    setAutoDelete( false );
}

void TaskCollect::run()
{

    LOG_INFO("\tFrom thread:{%d},\n"
             "\tuploading rendered image on the GPU of node:<%d> ...",
             QThread::currentThread(),vNode_.nodeId() );

    vNode_.uploadImage();

    LOG_INFO("\tFrom thread:{%d},\n"
             "\tuploading is DONE on the GPU of node:<%d> ...",
             QThread::currentThread(),vNode_.nodeId() );

}
