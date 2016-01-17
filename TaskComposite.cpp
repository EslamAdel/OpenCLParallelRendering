#include "TaskComposite.h"
#include <QThread>
#include <iostream>
#include "Logger.h"

TaskComposite::TaskComposite(const VirtualGPU &vGPU,
                             const CompositingMode compositingMode)
    : vGPU_( vGPU ),
      compositingMode_( compositingMode )
{
    setAutoDelete( false );
}

void TaskComposite::insertImage(const VirtualNode *vNode, const VirtualImage *vImage)
{
    nodesOutputImages_[ vNode ] = vImage ;
    if( nodesOutputImages_.size() != imagesStack_.size() )
        imagesStack_.push_back( vImage );

}

void TaskComposite::run()
{

    LOG_INFO("\tFrom thread:{%d}, Compositing Images..\n" ,
             QThread::currentThread());


    unsigned int i=0;
    for( auto &vImage : nodesOutputImages_ )
        imagesStack_[ i++ ] = vImage.second ;

    vGPU_.compositeImages( imagesStack_ );

    static int psuedoCount = 0 ;

    LOG_INFO("Frame #:%d is READY\n"
             "Compositing DONE. Flush to screen!\n"
             "------------------------", psuedoCount++ );



}

NodesOutputImages &TaskComposite::nodesOutputImages()
{
    return nodesOutputImages_;
}

