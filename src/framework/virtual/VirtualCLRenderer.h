#ifndef VirtualCLRenderer_H
#define VirtualCLRenderer_H

#include "CLRenderer.h"




class VirtualCLRenderer : public CLRenderer< uchar , uint >
{
public:
    VirtualCLRenderer( const uint64_t gpuIndex,
                       const uint frameWidth,
                       const uint frameHeight,
                       const Transformation &transformation );

    uint64_t getGPUIndex( ) const;


private:
    const uint64_t gpuIndex_VIRTUAL_ ;
};

#endif // VirtualCLRenderer_H
