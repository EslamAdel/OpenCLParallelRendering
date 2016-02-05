#ifndef PROFILINGEXTERNS_HH
#define PROFILINGEXTERNS_HH

#define BENCHMARKING

#include "Profiles.hh"



extern RenderingProfiles renderingProfiles ;
extern CollectingProfiles collectingProfiles ;
extern CompositingProfile compositingProfile;
extern FrameworkProfile frameworkProfile;


RenderingProfile &getRenderingProfile( RenderingProfiles &profiles , uint gpuIndex );


#endif // PROFILINGEXTERNS_HH

