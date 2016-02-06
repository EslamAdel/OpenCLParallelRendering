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

RenderingProfile &getRenderingProfile( RenderingProfiles &profiles ,
                                       RenderingNode *node  );

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

#define RENDERING_PROFILE_PASS_PTR( renderingNode )\
    getRenderingProfile( renderingProfiles , renderingNode )


#define COLLECTING_PROFILE( renderingNode )\
    getCollectingProfile( collectingProfiles , renderingNode )

#define PRINT( timer )\
    do{ timer.print( 1 );  \
    }while( 0 )


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

#define RENDEING_PROFILE_PASS_PTR( renderingNode )\
    do{  } \
    while( 0 )

#define COLLECTING_PROFILE( renderingNode )\
    do{  } \
    while( 0 )

#define PRINT( timer )\
    do{  }\
    while( 0 )

#endif


#endif // PROFILINGEXTERNS_HH

