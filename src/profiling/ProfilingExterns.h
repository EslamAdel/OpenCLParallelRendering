#ifndef PROFILINGEXTERNS_HH
#define PROFILINGEXTERNS_HH

#include "Profiles.hh"



#ifdef BENCHMARKING

#define TIC( timer ) \
    do{ timer.start(); } \
    while( 0 )

#define TOC( timer ) \
    do{ timer.stop(); } \
    while( 0 )


//return rendering profile by passing pointer of rendering node attached to.
#define RENDERING_PROFILE( renderer ) \
    getRenderingProfile( renderingProfiles , renderer )

//return collecting profile by passing pointer of rendering node attached to.
#define COLLECTING_PROFILE( renderer )\
    getCollectingProfile( collectingProfiles , renderer )

#define PRINT( timer )\
    do{ timer.print( 1 );  \
    }while( 0 )

#define RENDERING_PROFILE_TAG( renderer )\
    do{ printf("Statistics: Rendering on GPU <%d>\n" , renderer->getGPUIndex() ); }\
    while( 0 )

#define COLLECTING_PROFILE_TAG( renderer , compositor )\
    do{\
    printf("Statistics: Data Transfer from GPU <%d> --> Host --> GPU <%d>\n" ,\
    renderer->getGPUIndex() , compositor->getGPUIndex() ) ; }\
    while( 0 )

#define COMPOSITING_PROFILE_TAG( compositor )\
    do{\
    printf("Statistics: Compositing on GPU <%d>\n", compositor->getGPUIndex() ) ;}\
    while( 0 )

#define FRAMEWORK_PROFILE_TAG()\
    do{\
    printf("Statistics: framework\n"); }\
    while( 0 )

#define ATTACH_RENDERING_PROFILE( renderer )\
    do{\
    renderingProfiles[ renderer ] = new RenderingProfile ; }\
    while( 0 )

#define ATTACH_COLLECTING_PROFILE( renderer )\
    do{\
    collectingProfiles[ renderer ] = new CollectingProfile ; }\
    while( 0 )


#define DEFINE_PROFILES\
    RenderingProfiles renderingProfiles = RenderingProfiles() ; \
    CollectingProfiles collectingProfiles = CollectingProfiles() ; \
    CompositingProfile compositingProfile = CompositingProfile() ; \
    FrameworkProfile frameworkProfile = FrameworkProfile();

#define EXIT_PROFILING()\
    do{\
    exit(EXIT_SUCCESS);}\
    while( 0 )

#define BEEP()\
    do{\
    fprintf(stdout, "\a\n" );}\
    while( 0 )

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

#define RENDERING_PROFILE_TAG( node )\
    do{  }\
    while( 0 )

#define COLLECTING_PROFILE_TAG( node , compositingNode )\
    do{  }\
    while( 0 )

#define COMPOSITING_PROFILE_TAG( compositingNode )\
    do{  }\
    while( 0 )

#define FRAMEWORK_PROFILE_TAG()\
    do{  }\
    while( 0 )

#define ATTACH_RENDERING_PROFILE( renderingNode )\
    do{  }\
    while( 0 )

#define ATTACH_COLLECTING_PROFILE( renderingNode )\
    do{  }\
    while( 0 )

#define DEFINE_PROFILES


#define EXIT_PROFILING()\
    do{  }\
    while( 0 )

#define BEEP()\
    do{  }\
    while( 0 )

#endif




extern RenderingProfiles renderingProfiles ;
extern CollectingProfiles collectingProfiles ;
extern CompositingProfile compositingProfile;
extern FrameworkProfile frameworkProfile;
extern uint testFrames ;

RenderingProfile &getRenderingProfile( RenderingProfiles &profiles ,
                                       uint gpuIndex );

RenderingProfile &getRenderingProfile( RenderingProfiles &profiles ,
                                       CLAbstractRenderer *renderer  );

CollectingProfile &getCollectingProfile( CollectingProfiles &profile ,
                                         CLAbstractRenderer* renderer );



#endif // PROFILINGEXTERNS_HH

