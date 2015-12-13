#include "VirtualGPU.h"
#include <stdlib.h>

VirtualGPU::VirtualGPU()
{

}

double VirtualGPU::volumeRenderingTime( VirtualVolume &volume_ )
{
    return  volumeProcessingTime_( volume_ ) *
            VirtualExperiment::processScale( VirtualExperiment::ProcessOrder::Rendering );
}

double VirtualGPU::volumeProcessingTime_( VirtualVolume &volume_ )
{
    //non-linear
    return  parameters_.a * volume_.dim()[0] * volume_.dim()[1] * volume_.dim()[2] +
            parameters_.b * volume_.dim()[0] * volume_.dim()[1] +
            parameters_.c * volume_.dim()[0] * volume_.dim()[2] +
            parameters_.d * volume_.dim()[1] * volume_.dim()[2] +
            parameters_.e * volume_.dim()[0] +
            parameters_.f * volume_.dim()[1] +
            parameters_.g * volume_.dim()[2] ;
}


GPUParameters::GPUParameters()
{
    // a*(xyz) OR a*volume ... highest order. Should be with highest correlation with time

    a = drand48();

    // b,c,d ... 2nd order.
    b= drand48()/2;
    c= drand48()/2;
    d= drand48()/2;

    // e,f,g ... 1st order
    e= drand48()/4;
    f= drand48()/4;
    g= drand48()/4;

}
