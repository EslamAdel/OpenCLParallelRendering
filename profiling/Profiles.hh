#ifndef PROFILES_HH
#define PROFILES_HH

#include <oclHWDL.h>
#include "RenderingNode.h"
#include <unordered_map>

typedef oclHWDL::Timer64 Timer64 ;

struct RenderingProfile
{
    RenderingProfile() : threadSpawningTime_("Thread Spawning on Host") ,
                         transformationMatrix_("Transformation Matrix Calculation on Host") ,
                         rendering_("Rendering Frame on Device") {}

    Timer64 threadSpawningTime_ ;
    Timer64 transformationMatrix_ ;
    Timer64 rendering_ ;
};

struct CollectingProfile
{
    CollectingProfile() : threadSpawningTime_("Thread Spawning on Host") ,
                          loadingBufferFromDevice_("Loading Buffer From Device") ,
                          loadingBufferToDevice_("Loading Buffer To Device") {}
    Timer64 threadSpawningTime_ ;
    Timer64 loadingBufferFromDevice_ ;
    Timer64 loadingBufferToDevice_ ;

};

struct CompositingProfile
{
    CompositingProfile() : threadSpawningTime_("Thread Spawning on Host") ,
                           setKernelParameters_("Setting Kernel Arguments") ,
                           accumulatingFrameTime_("Accumulating Frame on Device") ,
                           compositingTime_("Total Compositing Time on Device") ,
                           loadCollageFromDeviceTime_("Loading Collage Buffer from Device") ,
                           rewindCollageTime_("Rewind Collage Buffer") {}
    Timer64 threadSpawningTime_;
    Timer64 setKernelParameters_;
    Timer64 accumulatingFrameTime_;
    Timer64 compositingTime_ ;
    Timer64 loadCollageFromDeviceTime_ ;
    Timer64 rewindCollageTime_;
};


struct FrameworkProfile
{
    FrameworkProfile() : convertToPixmapTime_("Converting to Pixmap on Host") ,
                         renderingLoopTimeWithoutPixmap_("Rendering Loop W/O Converting to Pixmap on Host"),
                         renderingLoopTime_("Rendering Loop") {}
    Timer64 convertToPixmapTime_;
    Timer64 renderingLoopTimeWithoutPixmap_ ;
    Timer64 renderingLoopTime_;
};


typedef std::unordered_map<const RenderingNode* , RenderingProfile* > RenderingProfiles;
typedef std::unordered_map<const RenderingNode* , CollectingProfile*> CollectingProfiles;
typedef std::unordered_map<const RenderingNode* , CompositingProfile*> CompositingProfiles;



#endif // PROFILES_HH
