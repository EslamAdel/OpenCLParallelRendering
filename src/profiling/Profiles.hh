#ifndef PROFILES_HH
#define PROFILES_HH

#include <oclHWDL.h>
#include "CLRenderer.h"
#include <unordered_map>

typedef oclHWDL::Timer64 Timer64 ;

struct GeneralProfile
{
    GeneralProfile() : threadSpawning_TIMER("Thread Spawning on Host") {}
    Timer64 threadSpawning_TIMER ;
};

struct RenderingProfile : public GeneralProfile
{
    RenderingProfile() : mvMatrix_TIMER("Transformation Matrix Calculation on Host") ,
                         rendering_TIMER("Rendering Frame on Device") {}
    Timer64 mvMatrix_TIMER ;
    Timer64 rendering_TIMER ;
};

struct CollectingProfile : public GeneralProfile
{
    CollectingProfile() : transferingBuffer_TIMER("Loading Buffer From Renderer to Compositor"){}
    Timer64 transferingBuffer_TIMER ;

};

struct CompositingProfile : public GeneralProfile
{
    CompositingProfile() : accumulatingFrame_TIMER("Accumulating Frame on Compositor") ,
                           compositing_TIMER("Total Compositing Time on Compositor") ,
                           loadFinalFromDevice_TIMER("Loading Final Buffer from Compositor"){}
    Timer64 threadSpawning_TIMER;
    Timer64 accumulatingFrame_TIMER;
    Timer64 compositing_TIMER ;
    Timer64 loadFinalFromDevice_TIMER ;
};


struct FrameworkProfile
{
    FrameworkProfile() : convertToPixmap_TIMER("Converting to Pixmap on Host") ,
                         renderingLoop_TIMER("Rendering Loop") {}
    Timer64 convertToPixmap_TIMER;
    Timer64 renderingLoop_TIMER;
};


typedef std::unordered_map< const CLAbstractRenderer* ,
                            RenderingProfile* > RenderingProfiles;

typedef std::unordered_map< const CLAbstractRenderer* ,
                            CollectingProfile* > CollectingProfiles;

typedef std::unordered_map< const CLAbstractRenderer* ,
                            CompositingProfile* > CompositingProfiles;



#endif // PROFILES_HH
