#include "Calibrator.h"

namespace clparen {
namespace Calibrator{

template< class V , class F >
Calibrator<V , F >::Calibrator(const uint64_t gpuIndex,
                               const uint frameWidth,
                               const uint frameHeight,
                               const Transformation &transformation,
                               const std::string kernelDirectory,
                               const uint iterations ,
                               QObject *parent )
    : AbstractCalibrator( parent ) ,
      rendererEngine_( gpuIndex, frameWidth, frameHeight,
                       transformation , kernelDirectory ),
      iterations_(iterations)
{
    connect( &rendererEngine_ ,
             SIGNAL( finishedRendering( Renderer::CLAbstractRenderer* )),
             this ,
             SLOT( finishRendering_SLOT(Renderer::CLAbstractRenderer*)));
    framesCounter_ = 0 ;
}

template <class V , class F >
void Calibrator< V ,F >::finishRendering_SLOT( Renderer::CLAbstractRenderer * )
{
    accumulator_.append( rendererEngine_.getRenderingTime( ));
    if( ++framesCounter_ == iterations_)
    {
        double meanTime = std::accumulate( accumulator_.begin(),
                                           accumulator_.end(),
                                           0.0) / accumulator_.size( );
        frameRate_ = 10e9/meanTime;

        emit finishCalibration_SIGNAL( frameRate_ ,
                                       rendererEngine_.getGPUIndex( ));
        framesCounter_ = 0 ;
    }
    else
        rendererEngine_.applyTransformation( );
}

}
}

#include "Calibrator.ipp"
