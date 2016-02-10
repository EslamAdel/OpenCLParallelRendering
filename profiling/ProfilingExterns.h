#ifndef PROFILINGEXTERNS_HH
#define PROFILINGEXTERNS_HH

#include "Profiles.hh"

#define BENCHMARKING

//#define PROFILE_SINGLE_GPU

#ifdef PROFILE_SINGLE_GPU
    #define DEPLOY_GPU_INDEX 1
#else
    #define PROFILE_ALL_GPUS
#endif


//#define VIRTUAL_GPUS 20

// As you increase TEST_FRAMES, standard deviations decrease.
#define TEST_FRAMES 5*360


//use: 256, 384, 512, 640, 768, 896, 1024
#define VOLUME_SIZE 1024

#define FRAME_WIDTH 512
#define FRAME_HEIGHT 512

//in bytes: 15.6 MB
#if  VOLUME_SIZE == 256
    #define VOLUME_PREFIX "/projects/volume-datasets/skull/skull"

//in bytes: 52.7 MB
#elif VOLUME_SIZE == 384
    #define VOLUME_PREFIX "/projects/volume-datasets/skull-scales/skull-384-384-384/output-voulme"

//in bytes: 125 MB
#elif VOLUME_SIZE == 512
    #define VOLUME_PREFIX "/projects/volume-datasets/skull-scales/skull-512-512-512/output-voulme"

//in bytes: 244.1 MB
#elif VOLUME_SIZE == 640
    #define VOLUME_PREFIX "/projects/volume-datasets/skull-scales/skull-640-640-640/output-voulme"

//in bytes: 421.8 MB
#elif VOLUME_SIZE == 768
    #define VOLUME_PREFIX "/projects/volume-datasets/skull-scales/skull-768-768-768/output-voulme"

//in bytes: 670 MB
#elif VOLUME_SIZE == 896
    #define VOLUME_PREFIX "/projects/volume-datasets/skull-scales/skull-896-896-896/output-voulme"

//in bytes: 1.0 GB
#elif VOLUME_SIZE == 1024
    #define VOLUME_PREFIX "/projects/volume-datasets/skull-scales/skull-1024-1024-1024/output-voulme"


#endif



#ifdef BENCHMARKING

#define TIC( timer ) \
    do{ timer.start(); } \
    while( 0 )

#define TOC( timer ) \
    do{ timer.stop(); } \
    while( 0 )


//return rendering profile by passing pointer of rendering node attached to.
#define RENDERING_PROFILE( renderingNode ) \
    getRenderingProfile( renderingProfiles , renderingNode )

//return collecting profile by passing pointer of rendering node attached to.
#define COLLECTING_PROFILE( renderingNode )\
    getCollectingProfile( collectingProfiles , renderingNode )

#define PRINT( timer )\
    do{ timer.print( 1 );  \
    }while( 0 )

#define RENDERING_PROFILE_TAG( node )\
    do{ printf("Statistics: Rendering on GPU <%d>\n" , node->getGPUIndex() ); }\
    while( 0 )

#define COLLECTING_PROFILE_TAG( node , compositingNode )\
    do{\
    printf("Statistics: Data Transfer from GPU <%d> --> Host --> GPU <%d>\n" ,\
    node->getGPUIndex() , compositingNode_->getGPUIndex() ) ; }\
    while( 0 )

#define COMPOSITING_PROFILE_TAG( compositingNode )\
    do{\
    printf("Statistics: Compositing on GPU <%d>\n", compositingNode_->getGPUIndex() ) ;}\
    while( 0 )

#define FRAMEWORK_PROFILE_TAG()\
    do{\
    printf("Statistics: framework\n"); }\
    while( 0 )

#define ATTACH_RENDERING_PROFILE( renderingNode )\
    do{\
    renderingProfiles[ renderingNode ] = new RenderingProfile ; }\
    while( 0 )

#define ATTACH_COLLECTING_PROFILE( renderingNode )\
    do{\
    collectingProfiles[ renderingNode ] = new CollectingProfile ; }\
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


RenderingProfile &getRenderingProfile( RenderingProfiles &profiles ,
                                       uint gpuIndex );

RenderingProfile &getRenderingProfile( RenderingProfiles &profiles ,
                                       RenderingNode *node  );

CollectingProfile &getCollectingProfile( CollectingProfiles &profile ,
                                         RenderingNode* node );



#endif // PROFILINGEXTERNS_HH

