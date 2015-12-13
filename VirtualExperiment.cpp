#include "VirtualExperiment.h"
#include <math.h>

VirtualExperiment::VirtualExperiment(const ProcessOrder processOrder)
    : processOrder_( processOrder )
{

}

double VirtualExperiment::processScale() const
{
    return pow( 10 , static_cast<double>(processOrder_ ) );
}

double VirtualExperiment::processScale(VirtualExperiment::ProcessOrder processOrder)
{
    return pow( 10 , static_cast<double>(processOrder ) );
}
