#include "VirtualCompositingNode.h"
#include "Logger.h"

VirtualCompositingNode::VirtualCompositingNode(const uint64_t gpuIndex,
                                                const uint frameWidth,
                                                const uint frameHeight)
    : CompositingNode( gpuIndex , frameWidth , frameHeight )
{

}

//void VirtualCompositingNode::collectFrame( VirtualRenderingNode *node,
//                                           const cl_bool block)
//{
//    LOG_DEBUG("Collecting..");


//    node->getCLFrame()->readDeviceData( node->getCommandQueue() ,
//                                        block );

//    this->frames_[ node ]->copyHostData( *node->getCLFrame() );
//    this->frames_[ node ]->writeDeviceData( this->commandQueue_ ,
//                                                  block );
//}

