#ifndef TIMER_H
#define TIMER_H
#include <sys/time.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <math.h>


template< class T >
class Timer
{

public:

    Timer( const std::string label );

    void start( );

    void stop( ) ;

    void print( const uint tabs );


private:

    void performStatistics_( T &mean , T &std , T &min ,
                             T &max ) ;

    void accumulate_( ) ;

    T duration_( ) const ;

private:
    timeval timer_[2];

    std::vector< T > data_ ;

    std::string label_;
};



struct RenderingProfile
{
    RenderingProfile() : threadSpawningTime_("Thread Spawning on Host") ,
                         transformationMatrix_("Transformation Matrix Calculation on Host") ,
                         rendering_("Rendering Frame on Device") {}

    Timer< double > threadSpawningTime_ ;
    Timer< double > transformationMatrix_ ;
    Timer< double > rendering_ ;
};

struct CollectingProfile
{
    CollectingProfile() : threadSpawningTime_("Thread Spawning on Host") ,
                          loadingBufferFromDevice_("Loading Buffer From Device") ,
                          loadingBufferToDevice_("Loading Buffer To Device") {}
    Timer< double > threadSpawningTime_ ;
    Timer< double > loadingBufferFromDevice_ ;
    Timer< double > loadingBufferToDevice_ ;

};

struct CompositingProfile
{
    CompositingProfile() : threadSpawningTime_("Thread Spawning on Host") ,
                           setKernelParameters_("Setting Kernel Arguments") ,
                           accumulatingFrameTime_("Accumulating Frame on Device") ,
                           compositingTime_("Total Compositing Time on Device") ,
                           loadCollageFromDeviceTime_("Loading Collage Buffer from Device") ,
                           rewindCollageTime_("Rewind Collage Buffer") {}
    Timer< double > threadSpawningTime_;
    Timer< double > setKernelParameters_;
    Timer< double > accumulatingFrameTime_;
    Timer< double > compositingTime_ ;
    Timer< double > loadCollageFromDeviceTime_ ;
    Timer< double > rewindCollageTime_;
};


struct FrameworkProfile
{
    FrameworkProfile() : renderingLoopTime_("Rendering Loop") {}
    Timer< double > renderingLoopTime_;

};

#endif // TIMER_H
