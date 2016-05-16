#ifndef CALIBRATOR_H
#define CALIBRATOR_H
#include "clparen.h"
#include <algorithm>

namespace clparen {
namespace calibrator{

template< class V ,  class F >
class Calibrator : public Renderer::CLRenderer< V , F >
{
public:
    Calibrator(const uint64_t gpuIndex,
               const uint frameWidth,
               const uint frameHeight,
               const Transformation &transformation,
               const std::string kernelDirectory,
               const uint iterations);
public slots:

    /**
    * @brief finishRendering_SLOT
    */
   void finishRendering_SLOT(Renderer::CLAbstractRenderer*);

signals:

   /**
    * @brief finishCalibration
    * @param frameRate
    */
   void finishCalibration(double frameRate , uint64_t gpuIdx);

private:

    /**
     * @brief iterations_
     */
    const uint iterations_;

    /**
     * @brief accumulator_
     */
    QVector< double > accumulator_;

    /**
     * @brief frameRate_
     */
    double frameRate_;
};
}
}
#endif // CALIBRATOR_H
