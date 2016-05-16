#include "Calibrator.h"
#include <Timer.h>

namespace clparen {
namespace Calibrator{

template< class V , class F >
Calibrator<V , F >::Calibrator( const uint64_t gpuIndex,
                                const uint frameWidth,
                                const uint frameHeight,
                                const Transformation transformation,
                                const std::string kernelDirectory,
                                const uint iterations ,
                                QObject *parent )
    : AbstractCalibrator( parent ) ,
      rendererEngine_( gpuIndex, frameWidth, frameHeight,
                       transformation , kernelDirectory ),
      iterations_( iterations )
{

}


template< class V , class F >
double Calibrator< V , F >::startCalibration()
{

    Timer64 timer;

    for( uint i = 0 ; i < iterations_ ; i++ )
    {
        timer.start();
        rendererEngine_.applyTransformation( );
        timer.stop();
    }

    double mean = timer.getMean();

    emit finishedCalibration_SIGNAL( mean ,
                                     rendererEngine_.getGPUIndex( ));

    return mean ;
}

template< class V , class F >
void Calibrator< V , F >::loadVolume(  const VolumeVariant &volume )
{
    rendererEngine_.loadVolume( volume );
}



}
}

#include "Calibrator.ipp"
