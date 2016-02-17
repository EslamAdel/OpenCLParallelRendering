#ifndef VIRTUALRENDERINGNODE_H
#define VIRTUALRENDERINGNODE_H

#include "RenderingNode.h"




class VirtualRenderingNode : public RenderingNode
{
public:
    VirtualRenderingNode( const uint64_t gpuIndex,
                          const uint frameWidth,
                          const uint frameHeight,
                          const Coordinates3D &globalTranslation,
                          const Coordinates3D &globalRotation,
                          const float &volumeDensity,
                          const float &brightness ,
                          const float &transferScale,
                          const float &transferOffset);

    uint64_t getGPUIndex( ) const;


private:
    const uint64_t gpuIndex_VIRTUAL_ ;
};

#endif // VIRTUALRENDERINGNODE_H
