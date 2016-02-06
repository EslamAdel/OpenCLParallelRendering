
#include "ProfilingExterns.h"

RenderingProfile &getRenderingProfile( RenderingProfiles &profiles , uint gpuIndex )
{
    for( auto it : profiles )
        if( it.first->getGPUIndex() == gpuIndex  )
            return *it.second;

}

CollectingProfile &getCollectingProfile(CollectingProfiles &profile,
                                        RenderingNode *node)
{
    return *profile[ node ];
}
