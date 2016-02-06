#ifndef PROFILINGEXTERNS_HH
#define PROFILINGEXTERNS_HH

#define BENCHMARKING

#include "Profiles.hh"



extern RenderingProfiles renderingProfiles ;
extern CollectingProfiles collectingProfiles ;
extern CompositingProfile compositingProfile;
extern FrameworkProfile frameworkProfile;


RenderingProfile &getRenderingProfile( RenderingProfiles &profiles , uint gpuIndex );

#ifdef BENCHMARKING

#define RENDERING_THREAD_START( RenderingNodePtr ) \
    do{ renderingProfiles[ RenderingNodePtr ]->threadSpawningTime_.start(); } \
    while( 0 )

#define RENDERING_THREAD_STOP( RenderingNodePtr ) \
    do{ renderingProfiles[ RenderingNodePtr ]->threadSpawningTime_.stop(); } \
    while( 0 )

#define RENDERIN_MATRIX_START( RenderingNodePtr )\
    do{ renderingProfiles[ RenderingNodePtr ]->transformationMatrix_.start(); } \
    while( 0 )

#define RENDERIN_MATRIX_STOP( RenderingNodePtr )\
    do{ renderingProfiles[ RenderingNodePtr ]->transformationMatrix_.stop(); } \
    while( 0 )
#else
#define RENDERING_PROF_THREAD_START( RenderingNodePtr ) \
    do{ } while( 0 )

#endif

#ifdef BEN

#endif

#endif // PROFILINGEXTERNS_HH

