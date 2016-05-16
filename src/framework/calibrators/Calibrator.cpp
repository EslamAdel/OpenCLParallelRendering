#include "Calibrator.h"
#include "CLRenderer.h"

namespace clparen {
namespace calibrator{

template< class V , class F >
Calibrator<V , F >::Calibrator(const uint64_t gpuIndex,
                               const uint frameWidth,
                               const uint frameHeight,
                               const Transformation &transformation,
                               const std::string kernelDirectory,
                               const uint iterations)
    : Renderer::CLRenderer< V , F >( gpuIndex, frameWidth, frameHeight,
                                     transformation , kernelDirectory),
      iterations_(iterations)
{
    connect(this,SIGNAL(finishedRendering(Renderer::CLAbstractRenderer*)),
                        this,SLOT(finishRendering_SLOT(Renderer::CLAbstractRenderer*)));
}

template <class V , class F >
void Calibrator< V ,F >::finishRendering_SLOT(Renderer::CLAbstractRenderer *)
{
    static int i = 0;
    accumulator_.append(this->getRenderingTime());
    if(++i == iterations_)
    {
        double meanTime = std::accumulate(accumulator_.begin(),
                                          accumulator_.end(),
                                          0.0)/accumulator_.size();
        frameRate_ = 10e9/meanTime;
        emit this->finishCalibration(frameRate_ , this->getGPUIndex());
    }
    else
    {
        this->applyTransformation();
    }
}

}}

