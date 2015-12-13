#include "VirtualNode.h"

VirtualNode::VirtualNode()
{
    volumeExist_ = false;
}

void VirtualNode::setVolume(const VirtualVolume &volume)
{
    subVolume_ = volume ;
    volumeExist_ = true;
}

bool VirtualNode::volumeExist() const
{
    return volumeExist_;
}

