#include "RenderingNode.h"
#include <Logger.h>

RenderingNode::RenderingNode(const uint64_t gpuIndex ,
                             const uint frameWidth, const uint frameHeight,
                             Coordinates3D &globalTranslation,
                             Coordinates3D &globalRotation,
                             float &volumeDensity, float &brightness,
                             const Volume<uchar> *subVolume)
    : gpuIndex_( gpuIndex ),
      frameWidth_( frameWidth ),
      frameHeight_( frameHeight ),
      translation_( globalTranslation ),
      rotation_( globalRotation ),
      volumeDensity_( volumeDensity ),
      imageBrightness_( brightness )
{
    LOG_INFO( "Creating Context on Node with GPU <%d>", gpuIndex );

    clContext_ = new CLContext< uint8_t >( subVolume , gpuIndex ,
                                           frameWidth , frameHeight );

}

void RenderingNode::loadVolume(const Volume<uchar> *subVolume)
{
    clContext_->loadVolume( subVolume );
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

}

void RenderingNode::frameBufferToPixmap()
{
    clContext_->frameBufferToPixmap();

}

const uint64_t &RenderingNode::gpuIndex() const
{
    return clContext_->getGPUIndex();
}

bool RenderingNode::operator==( const RenderingNode &rhs) const
{
    return (gpuIndex_ == rhs.gpuIndex());
}

CLContext<uint8_t> *RenderingNode::getContext() const
{
    return clContext_;
}

