#include "VirtualFrameWork.h"

VirtualFrameWork::VirtualFrameWork()
{

}


void VirtualFrameWork::distributeVolume_()
{
    qDebug() << "void VirtualFrameWork::distributeVolume_()" ;
    if( nodes_.isEmpty() ) return;

    int nNodes = nodes_.count();

    QList<VirtualVolume *> subVolumes = mainVolume_.subVolumes1D( nNodes ) ;

    VirtualVolume *subVolume = subVolumes.begin();
    foreach ( VirtualNode *node , nodes_ )
    {
        node->setVolume( subVolume );
        subVolume++;
    }
}
