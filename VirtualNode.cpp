#include "VirtualNode.h"
#include <stdio.h>

VirtualNode::VirtualNode()
{
    volumeExist_ = false;

    connect( &vGPU_ ,
             SIGNAL(finishedRendering()),
             this,
             SLOT(slotFinishedRendering()) );

    connect( &vGPU_ ,
             SIGNAL(finishedCompositing()),
             this,
             SLOT(slotFinishedCompositing()));
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

void VirtualNode::slotFinishedRendering()
{
    emit this->finishedRendering( this );
}

void VirtualNode::slotFinishedCompositing()
{
    emit this->finishedCompositing( this );
}

