#ifndef VIRTUALCOMPOSITINGNODE_H
#define VIRTUALCOMPOSITINGNODE_H

#include "CompositingNode.h"
#include "VirtualRenderingNode.h"

class VirtualCompositingNode : public CompositingNode
{
public:
    VirtualCompositingNode( const uint64_t gpuIndex ,
                            const uint frameWidth ,
                            const uint frameHeight );


//    void collectFrame( const uint8_t frameIndex ,
//                       cl_command_queue sourceCmdQueue ,
//                       CLFrame32 &sourceFrame ,
//                       const cl_bool block );
};

#endif // VIRTUALCOMPOSITINGNODE_H
