#include "TaskComposite.h"
#include <QThread>
#include <iostream>

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

    std::cout << "\tfrom thread:"<<QThread::currentThread()<<", Compositing Images.."<<std::endl;

    unsigned int i=0;
    for( auto &vImage : nodesOutputImages_ )
        imagesStack_[ i++ ] = vImage.second ;

    vGPU_.compositeImages( imagesStack_ );

    static int psuedoCount = 0 ;
    std::cout << "Frame #:" << psuedoCount++ <<" is READY!\n" ;

    std::cout <<"Compositing DONE. Flush to screen!"<<std::endl;



}

NodesOutputImages &TaskComposite::nodesOutputImages()
{
    return nodesOutputImages_;
}

