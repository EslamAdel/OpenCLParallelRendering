#ifndef PROFILES_HH
#define PROFILES_HH

#include <oclHWDL.h>
#include "RenderingNode.h"
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
    CollectingProfile() : loadingBufferFromDevice_TIMER("Loading Buffer From Device") ,
                          loadingBufferToDevice_TIMER("Loading Buffer To Device") {}
    Timer64 loadingBufferFromDevice_TIMER ;
    Timer64 loadingBufferToDevice_TIMER ;

};

struct CompositingProfile : public GeneralProfile
{
    CompositingProfile() : accumulatingFrame_TIMER("Accumulating Frame on Device") ,
                           compositing_TIMER("Total Compositing Time on Device") ,
                           loadCollageFromDevice_TIMER("Loading Collage Buffer from Device"){}
    Timer64 threadSpawning_TIMER;
    Timer64 accumulatingFrame_TIMER;
    Timer64 compositing_TIMER ;
    Timer64 loadCollageFromDevice_TIMER ;
};


struct FrameworkProfile
{
    FrameworkProfile() : convertToPixmap_TIMER("Converting to Pixmap on Host") ,
                         renderingLoop_TIMER("Rendering Loop") {}
    Timer64 convertToPixmap_TIMER;
    Timer64 renderingLoop_TIMER;
};


typedef std::unordered_map<const RenderingNode* , RenderingProfile* > RenderingProfiles;
typedef std::unordered_map<const RenderingNode* , CollectingProfile*> CollectingProfiles;
typedef std::unordered_map<const RenderingNode* , CompositingProfile*> CompositingProfiles;



#endif // PROFILES_HH
