
#include "ProfilingExterns.h"

RenderingProfile &getRenderingProfile( const RenderingProfiles &profiles ,
                                       const uint gpuIndex )
{
    RenderingProfiles::const_iterator it = profiles.begin();

    while( it != profiles.end( ))
    {
        if( it.key()->getGPUIndex() == gpuIndex  )
            return *it.value();
        it++ ;
    }


}

CollectingProfile &getCollectingProfile(
        const CollectingProfiles &profiles ,
        const clpar::Renderer::CLAbstractRenderer *renderer)
{
    return *profiles.value( renderer );
}



RenderingProfile &getRenderingProfile(
        const RenderingProfiles &profiles,
        const clpar::Renderer::CLAbstractRenderer *renderer)
{
    return *profiles.value( renderer ) ;
}


