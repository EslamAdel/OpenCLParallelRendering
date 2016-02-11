#ifndef VIRTUALCOMPOSITINGNODE_H
#define VIRTUALCOMPOSITINGNODE_H

#include "CompositingNode.h"

class VirtualCompositingNode : public CompositingNode
{
public:
    VirtualCompositingNode( const uint64_t gpuIndex ,
                            const uint framesCount ,
                            const uint frameWidth ,
                            const uint frameHeight ,
                            const std::vector< const Coordinates3D* > framesCenters );


    void collectFrame( const uint8_t frameIndex ,
                       cl_command_queue sourceCmdQueue ,
                       CLFrame32 &sourceFrame ,
                       const cl_bool block );
};

#endif // VIRTUALCOMPOSITINGNODE_H
