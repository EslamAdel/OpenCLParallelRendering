
#include "ProfilingExterns.h"

RenderingProfile &getRenderingProfile( RenderingProfiles &profiles , uint gpuIndex )
{
    for( auto it : profiles )
        if( it.first->getGPUIndex() == gpuIndex  )
            return *it.second;

}

CollectingProfile &getCollectingProfile( CollectingProfiles &profiles ,
                                         CLRenderer *renderer)
{
    return *profiles[ renderer ];
}



RenderingProfile &getRenderingProfile( RenderingProfiles &profiles,
                                       CLRenderer *renderer)
{
    return *profiles[ renderer ] ;
}


