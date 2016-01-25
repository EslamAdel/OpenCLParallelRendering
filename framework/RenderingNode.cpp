#include "RenderingNode.h"
#include <Logger.h>

RenderingNode::RenderingNode(const uint64_t gpuIndex ,
                             const Volume<uchar> *subVolume)
    : gpuIndex_( gpuIndex )
{
    LOG_INFO( "Creating Context on Node with GPU <%>", gpuIndex );

    clContext_ = new CLContext< uint8_t >( subVolume , gpuIndex );

}

void RenderingNode::loadVolume(const Volume<uchar> *subVolume)
{
    clContext_->loadNewVolume( subVolume );
}

void RenderingNode::setTransformations(const Coordinates3D rotation,
                                       const Coordinates3D translation,
                                       const float volumeDensity,
                                       const float imageBrightness)
{
    rotation_ = rotation;
    translation_ = translation;
    volumeDensity_ = volumeDensity;
    imageBrightness_ = imageBrightness;
}

void RenderingNode::applyTransformation( )
{

    clContext_->paint( rotation_,
                       translation_,
                       volumeDensity_,
                       imageBrightness_ );

    emit this->finishedRendering( this );
}

void RenderingNode::uploadBuffer()
{
    clContext_->uploadFrame();

    emit this->bufferUploaded( this );
}

const uint64_t &RenderingNode::gpuIndex() const
{
    return gpuIndex_;
}

bool RenderingNode::operator==( const RenderingNode &rhs) const
{
    return (gpuIndex_ == rhs.gpuIndex());
}

CLContext<uint8_t> *RenderingNode::getContext() const
{
    return clContext_;
}

