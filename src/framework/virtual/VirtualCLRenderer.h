#ifndef VirtualCLRenderer_H
#define VirtualCLRenderer_H

#include "CLRenderer.h"




class VirtualCLRenderer : public CLRenderer
{
public:
    VirtualCLRenderer( const uint64_t gpuIndex,
                          const uint frameWidth,
                          const uint frameHeight,
                          const Coordinates3D &globalTranslation,
                          const Coordinates3D &globalRotation,
                          const Coordinates3D &globalScale,
                          const float &volumeDensity,
                          const float &brightness ,
                          const float &transferScale,
                          const float &transferOffset);

    uint64_t getGPUIndex( ) const;


private:
    const uint64_t gpuIndex_VIRTUAL_ ;
};

#endif // VirtualCLRenderer_H
