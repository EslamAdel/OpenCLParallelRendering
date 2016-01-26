#include "RenderingNode.h"
#include <Logger.h>

RenderingNode::RenderingNode(const uint64_t gpuIndex ,
                             Coordinates3D &globalTranslation,
                             Coordinates3D &globalRotation,
                             float &volumeDensity, float &brightness,
                             const Volume<uchar> *subVolume)
    : gpuIndex_( gpuIndex ),
      translation_( globalTranslation ),
      rotation_( globalRotation ),
      volumeDensity_( volumeDensity ),
      imageBrightness_( brightness )
{
    LOG_INFO( "Creating Context on Node with GPU <%d>", gpuIndex );

    clContext_ = new CLContext< uint8_t >( subVolume , gpuIndex );

}

void RenderingNode::loadVolume(const Volume<uchar> *subVolume)
{
    clContext_->loadNewVolume( subVolume );
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

    //emit this->bufferUploaded( this );
}

void RenderingNode::frameBufferToPixmap()
{
    clContext_->frameBufferToPixmap();

    emit this->bufferUploaded( this );
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

