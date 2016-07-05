#include "Calibrator.h"
#include <Timer.h>

namespace clparen {
namespace Calibrator{

template< class V , class F >
Calibrator<V , F >::Calibrator( const uint64_t gpuIndex,
                                const uint frameWidth,
                                const uint frameHeight,
                                const Transformation transformation,
                                const uint iterations ,
                                QObject *parent )
    : AbstractCalibrator( parent ) ,
      transformation_( transformation ) ,
      rendererEngine_( gpuIndex, transformation_ ,
                       Dimensions2D( frameWidth, frameHeight )),
      iterations_( iterations )
{

}


template< class V , class F >
double Calibrator< V , F >::startCalibration()
{

    Timer64 timer ;
    QVector< double > renderingTimes ;
    for( uint i = 0 ; i < iterations_ ; i++ )
    {
        timer.start();
        transformation_.rotation.x++;
        transformation_.rotation.y++;
        transformation_.rotation.z++;
        rendererEngine_.applyTransformation( );
        timer.stop();

        renderingTimes << rendererEngine_.getRenderingTime();
    }


    double mean = 0 ;
    const uint size = renderingTimes.size() ;

    for( const double time : renderingTimes )
        mean += time / size ;

    return mean ;
}

template< class V , class F >
void Calibrator< V , F >::loadVolume( VolumeVariant &volume )
{
    rendererEngine_.loadVolume( volume.value< Volume< V > *>() );
}

template< class V , class F >
std::string Calibrator< V , F >::getGPUName() const
{
    return rendererEngine_.getGPUName();
}



}
}

#include "Calibrator.ipp"
