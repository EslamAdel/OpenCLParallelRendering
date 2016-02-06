#ifndef PROFILINGEXTERNS_HH
#define PROFILINGEXTERNS_HH

#define BENCHMARKING

#include "Profiles.hh"



extern RenderingProfiles renderingProfiles ;
extern CollectingProfiles collectingProfiles ;
extern CompositingProfile compositingProfile;
extern FrameworkProfile frameworkProfile;


RenderingProfile &getRenderingProfile( RenderingProfiles &profiles ,
                                       uint gpuIndex );

CollectingProfile &getCollectingProfile( CollectingProfiles &profile ,
                                         RenderingNode* node );

#ifdef BENCHMARKING

#define TIC( timer ) \
    do{ timer.start(); } \
    while( 0 )

#define TOC( timer ) \
    do{ timer.stop(); } \
    while( 0 )
#define RENDERING_PROFILE( gpuIndex )\
    getRenderingProfile( renderingProfiles , gpuIndex )

#define COLLECTING_PROFILE( renderingNode )\
    getCollectingProfile( collectingProfiles , renderingNode )

#else
#define TIC( timer ) \
    do{  } \
    while( 0 )

#define TOC( timer ) \
    do{  } \
    while( 0 )

#define RENDERING_PROFILE( gpuIndex )\
    do{  } \
    while( 0 )

#define COLLECTING_PROFILE( renderingNode )\
    do{  } \
    while( 0 )

#endif


#endif // PROFILINGEXTERNS_HH

