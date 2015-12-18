#include "VirtualNode.h"
#include <iostream>


VirtualNode::VirtualNode(const int id)
    : nodeId_(id)
{
    volumeExist_ = false;

    connect( &vGPU_ ,
             SIGNAL(finishedRendering()),
             this,
             SLOT(slotFinishedRendering()) );

//    connect( &vGPU_ ,
//             SIGNAL(finishedCompositing()),
//             this,
//             SLOT(slotFinishedCompositing()));

}

void VirtualNode::setVolume(const VirtualVolume &volume)
{
    subVolume_ = volume ;
    volumeExist_ = true;
    vGPU_.loadVolume( &subVolume_ );
}

bool VirtualNode::volumeExist() const
{
    return volumeExist_;
}

void VirtualNode::initialRendering()
{
    if( volumeExist() == false )
    {
        std::cout << "No volume exists!\n";
        return;
    }
    startRendering();
}

void VirtualNode::startRendering()
{
    if( volumeExist() == false )
    {
        std::cout << "No volume exists!\n";
        return;
    }

    vGPU_.renderVolume();
}

void VirtualNode::applyTransformation(const Transformation *transformation)
{
    if( volumeExist() == false )
    {
        std::cout << "No volume exists!\n";
        return;
    }

    vGPU_.applyTransformation( transformation );
}

void VirtualNode::uploadImage()
{
    uploadedResultantImage_ = vGPU_.resultantImage();

    emit this->imageUploaded( this );
}

const VirtualImage *VirtualNode::uploadedResultantImage() const
{
    return uploadedResultantImage_;
}

const int VirtualNode::nodeId() const
{
    return nodeId_;
}

void VirtualNode::slotFinishedRendering()
{
    emit this->finishedRendering( this );
}

//void VirtualNode::slotFinishedCompositing()
//{
//    uploadedResultantImage_ = vGPU_.resultantImage();

//    emit this->finishedCompositing( this );
//}
