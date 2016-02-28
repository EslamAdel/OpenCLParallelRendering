#ifndef VirtualCLRenderer_H
#define VirtualCLRenderer_H

#include "CLRenderer.h"



template< class V ,  class F >
class VirtualCLRenderer : public CLRenderer< V , F >
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
