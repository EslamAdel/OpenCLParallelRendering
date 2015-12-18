#include "VirtualFrameWork.h"
#include <iostream>
#include <QDebug>
VirtualFrameWork::VirtualFrameWork(VirtualVolume &volume,
                                   const TaskComposite::CompositingMode compositingMode,
                                   const VirtualFrameWork::FrameWorkMode frameWorkMode)
    : mainVolume_( volume ),
      volumeDistributed_(false),
      blockNewNodes_(false),
      frameWorkmode_( frameWorkMode )
{
    taskComposite_ = new TaskComposite( serverGPU_, compositingMode);

    connect( &serverGPU_,
             SIGNAL(finishedCompositing()),
             this,
             SLOT(slotNodeFinishedCompositing()));
#ifdef DEMO
    std::cout << "Number of virtual nodes:";
    int n=0;
    std::cin >> n;
    for(auto i=0; i < n ; i++ ) addVirtualNode();


    connect( this ,
             SIGNAL(blockTransformations(bool)),
             this,
             SLOT(slotApplyNewTransformation(bool)));

    char startRendering = 'n';
    std::cout <<"Start Rendering?<y/n>:";
    std::cin >> startRendering ;

    if(startRendering == 'y') this->startRendering();
    else return ;

#endif


}


#ifdef DEMO
void VirtualFrameWork::slotApplyNewTransformation(bool blockTransformation)
{
    if(!blockTransformation)
    {
        slotNewTransformations();
    }

}
#endif
void VirtualFrameWork::addVirtualNode()
{
    if( blockNewNodes_ == true )
    {
        std::cout << "Adding new nodes is blocked!\n";
        exit(EXIT_FAILURE);
    }

    static int nNodes = 1 ;

    std::cout << "Adding node #:"<<nNodes<< std::endl;

    VirtualNode *newNode = new VirtualNode(nNodes);

    TaskRender *taskRender = new TaskRender( *newNode );

    TaskCollect *taskCollect = new TaskCollect( *newNode );


    nodes_.append( newNode );
    nodesRenderTasks_[ newNode ] = taskRender;
    nodesCollectTasks_[ newNode ] = taskCollect ;

    connect( newNode,
             SIGNAL(finishedRendering(VirtualNode*)),
             this,
             SLOT(slotNodeFinishedRendering(VirtualNode*)));
    connect( newNode,
             SIGNAL(imageUploaded(VirtualNode*)),
             this,
             SLOT(slotNodeImageUploaded(VirtualNode*)));

    nNodes++;
}

void VirtualFrameWork::startRendering(const Transformation *transformation)
{
    transformationsBlocked_ = true;
    emit this->blockTransformations(true);


    if(volumeDistributed_ == false )
    {
        distributeVolume_();
        std::cout <<"Volume distributed!"<<std::endl;
        volumeDistributed_ = true;
    }

    foreach( VirtualNode *vNode , nodes_)
    {
        renderer_.start( nodesRenderTasks_[ vNode ] );
    }

}

void VirtualFrameWork::applyTramsformation(const Transformation *transformation )
{
    startRendering( transformation );
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

    static int psuedoCount = 0 ;
    std::cout << "Frame #:" << psuedoCount++ << std::endl ;

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

void VirtualFrameWork::slotNodeFinishedCompositing()
{
    transformationsBlocked_ = false ;
    emit this->blockTransformations( false );
    flushScreen_();
}

void VirtualFrameWork::slotNodeImageUploaded(VirtualNode *vNode)
{
    static int nUploadedImages = 0;

    nUploadedImages++;

    taskComposite_->insertImage( vNode , vNode->uploadedResultantImage() );

    if( nUploadedImages == nodes_.size() )
    {
        compositor_.start( taskComposite_ );
        nUploadedImages = 0 ;
    }
}

void VirtualFrameWork::slotNewTransformations()
{
    applyTramsformation( &globalTransformation_ );
}
