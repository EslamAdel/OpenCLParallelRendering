#ifndef TIMER_H
#define TIMER_H
#include <sys/time.h>
#include <cstdlib>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/min.hpp>



using namespace boost::accumulators ;

typedef accumulator_set< double, features< tag::mean , tag::variance ,
                                           tag::min , tag::max >> Statistics;

class Timer
{

public:

    Timer( const std::string label );

    void start( );

    void stop( ) ;

    void print( const uint tabs );

private:
    void accumulate_( ) ;

    double duration_( ) const ;

private:
    timeval timer_[2];
    Statistics statistics_;
    std::string label_;
};


struct RenderingProfile
{
    RenderingProfile() : threadSpawningTime_("Thread Spawning on Host") ,
                         transformationMatrix_("Transformation Matrix Calculation on Host") ,
                         rendering_("Rendering Frame on Device") {}

    Timer threadSpawningTime_ ;
    Timer transformationMatrix_ ;
    Timer rendering_ ;
};

struct CollectingProfile
{
    CollectingProfile() : threadSpawningTime_("Thread Spawning on Host") ,
                          loadingBufferFromDevice_("Loading Buffer From Device") ,
                          loadingBufferToDevice_("Loading Buffer To Device") {}
    Timer threadSpawningTime_ ;
    Timer loadingBufferFromDevice_ ;
    Timer loadingBufferToDevice_ ;

};

struct CompositingProfile
{
    CompositingProfile() : threadSpawningTime_("Thread Spawning on Host") ,
                           setKernelParameters_("Setting Kernel Arguments") ,
                           accumulatingFrameTime_("Accumulating Frame on Device") ,
                           compositingTime_("Total Compositing Time on Device") ,
                           loadCollageFromDeviceTime_("Loading Collage Buffer from Device") ,
                           rewindCollageTime_("Rewind Collage Buffer") {}
    Timer threadSpawningTime_;
    Timer setKernelParameters_;
    Timer accumulatingFrameTime_;
    Timer compositingTime_ ;
    Timer loadCollageFromDeviceTime_ ;
    Timer rewindCollageTime_;
};


struct FrameworkProfile
{
    FrameworkProfile() : renderingLoopTime_("Rendering Loop") {}
    Timer renderingLoopTime_;

};

#endif // TIMER_H
