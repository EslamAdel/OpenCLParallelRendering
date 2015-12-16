#include "VirtualFrameWork.h"
#include <iostream>
#include <QDebug>

VirtualFrameWork::VirtualFrameWork(VirtualVolume &volume,
                                   const TaskComposite::CompositingMode compositingMode,
                                   const VirtualFrameWork::FrameWorkMode frameWorkMode)
    : mainVolume_( volume ),
      frameWorkmode_( frameWorkMode )
{
    taskComposite_ = new TaskComposite( serverGPU_, compositingMode);
}

void VirtualFrameWork::addVirtualNode()
{
    if( blockNewNodes_ = true )
    {
        std::cout << "Adding new nodes is blocked!\n";
        exit(EXIT_FAILURE);
    }

    VirtualNode *newNode = new VirtualNode();

    TaskRender *taskRender = new TaskRender( *newNode );

    TaskCollect *taskCollect = new TaskCollect( *newNode );


    nodes_.append( newNode );
    nodesRenderTasks_[ newNode ] = taskRender;
    nodesCollectTasks_[ newNode ] = taskCollect ;

    connect( newNode ,
             SIGNAL(finishedCompositing(VirtualNode*)),
             this,
             SLOT(slotNodeFinishedCompositing(VirtualNode*)));
    connect( newNode,
             SIGNAL(finishedRendering(VirtualNode*)),
             this,
             SLOT(slotNodeFinishedRendering(VirtualNode*)));
    connect( newNode,
             SIGNAL(imageUploaded(VirtualNode*)),
             this,
             SLOT(slotNodeImageUploaded(VirtualNode*)));

}

void VirtualFrameWork::startRendering()
{
    foreach( VirtualNode *vNode , nodes_)
    {
        renderer_.start( nodesRenderTasks_[ vNode ] );
    }

    transformationsBlocked_ = true;
    emit this->blockTransformations(true);
}

Transformation &VirtualFrameWork::globalTransformation()
{
    return globalTransformation_;
}

bool VirtualFrameWork::transformationsBlocked() const
{
    return transformationsBlocked_;
}

void VirtualFrameWork::flushScreen_()
{

}

void VirtualFrameWork::distributeVolume_()
{
    qDebug() << "void VirtualFrameWork::distributeVolume_()" ;
    if( nodes_.isEmpty() ) return;

    int nNodes = nodes_.count();

    QList<VirtualVolume *> *subVolumes = mainVolume_.subVolumes1D( nNodes ) ;

    QList<VirtualVolume*>::Iterator subVolume = subVolumes->begin();
    foreach ( VirtualNode *node , nodes_ )
    {
        node->setVolume( **subVolume );
        subVolume++;
    }

    blockNewNodes_ = true;
    emit this->blockNewNodes(blockNewNodes_);
}

void VirtualFrameWork::slotNodeFinishedRendering(VirtualNode *vNode)
{
    collector_.start( nodesCollectTasks_[ vNode ] );
}

void VirtualFrameWork::slotNodeFinishedCompositing(VirtualNode *vNode)
{
    transformationsBlocked_ = false ;

    flushScreen_();
}

void VirtualFrameWork::slotNodeImageUploaded(VirtualNode *vNode)
{

}

void VirtualFrameWork::slotNewTransformations()
{

}
