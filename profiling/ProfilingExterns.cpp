
#include "ProfilingExterns.h"

RenderingProfile &getRenderingProfile( RenderingProfiles &profiles , uint gpuIndex )
{
    for( auto it : profiles )
        if( it.first->getGPUIndex() == gpuIndex  )
            return *it.second;

}

CollectingProfile &getCollectingProfile( CollectingProfiles &profiles ,
                                         RenderingNode *node)
{
    return *profiles[ node ];
}



RenderingProfile &getRenderingProfile( RenderingProfiles &profiles,
                                       RenderingNode *node)
{
    return *profiles[ node ] ;
}


