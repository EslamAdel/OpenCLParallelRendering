#include "TaskCollect.h"
#include <iostream>
#include <QThread>

TaskCollect::TaskCollect( VirtualNode &vNode)
    : vNode_( vNode )
{
    setAutoDelete( false );
}

void TaskCollect::run()
{
    std::cout <<"\tfrom thread:"<< QThread::currentThread();
    std::cout <<", uploading rendered image on the GPU of node:"<<vNode_.nodeId()<<" ...\n";

    vNode_.uploadImage();

    std::cout <<"\tfrom thread:"<< QThread::currentThread();
    std::cout <<", uploading DONE on the GPU of node:"<<vNode_.nodeId()<<std::endl;
}
