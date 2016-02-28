
#include "ProfilingExterns.h"

RenderingProfile &getRenderingProfile( const RenderingProfiles &profiles ,
                                       const uint gpuIndex )
{
    for( auto it : profiles )
        if( it.first->getGPUIndex() == gpuIndex  )
            return *it.second;

}

CollectingProfile &getCollectingProfile( const CollectingProfiles &profiles ,
                                         const CLAbstractRenderer *renderer)
{
    return *profiles.at( renderer );
}



RenderingProfile &getRenderingProfile( const RenderingProfiles &profiles,
                                       const CLAbstractRenderer *renderer)
{
    return *profiles.at( renderer ) ;
}


