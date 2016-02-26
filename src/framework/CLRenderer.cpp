#include "CLRenderer.h"
#include <Logger.h>

CLRenderer::CLRenderer( const uint64_t gpuIndex ,
                        const uint frameWidth, const uint frameHeight,
                        const Transformation &transformation )
    : CLContext< uchar >( gpuIndex , frameWidth , frameHeight , nullptr ) ,
      transformation_( transformation )
{
    LOG_INFO( "Creating Context on Node with GPU <%d>", gpuIndex );


}


void CLRenderer::applyTransformation( )
{
    this->paint( transformation_ ,  currentCenter_  );

    emit this->finishedRendering( this );
}

const Coordinates3D &CLRenderer::getCurrentCenter() const
{
    return currentCenter_ ;
}


void CLRenderer::loadVolume(const Volume<uchar> *volume)
{
    this->loadVolume_( volume );
    currentCenter_ = volume->getUnitCubeCenter();
}

void CLRenderer::setFrameIndex(const uint frameIndex)
{
    frameIndex_ = frameIndex;
}

uint CLRenderer::getFrameIndex() const
{
    return frameIndex_;
}

void CLRenderer::setTransferFunctionFlag(int flag)
{
    activeRenderingKernel_->setTransferFunctionFlag(flag);
}
