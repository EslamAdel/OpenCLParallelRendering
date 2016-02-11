#include "VirtualCompositingNode.h"
#include "Logger.h"

VirtualCompositingNode::VirtualCompositingNode( const uint64_t gpuIndex,
                                                const uint framesCount,
                                                const uint frameWidth,
                                                const uint frameHeight,
                                                const std::vector<const Coordinates3D *> framesCenters)
    : CompositingNode( gpuIndex , framesCount , frameWidth , frameHeight ,
                       framesCenters )
{

}

void VirtualCompositingNode::collectFrame( const uint8_t frameIndex,
                                           cl_command_queue sourceCmdQueue,
                                           CLFrame32 &sourceFrame,
                                           const cl_bool block)
{
    LOG_DEBUG("Collecting..");

    sourceFrame.readDeviceData( sourceCmdQueue ,
                                block );

    this->frames_[ frameIndex ]->copyHostData( sourceFrame.getHostData() );
    this->frames_[ frameIndex ]->writeDeviceData( this->commandQueue_ ,
                                                  block );
}

