#ifndef CALIBRATOR_H
#define CALIBRATOR_H
#include "clparen.h"
#include <algorithm>
#include "CLRenderer.h"
#include "AbstractCalibrator.h"

namespace clparen {
namespace Calibrator{

template< class V ,  class F >
class Calibrator : public AbstractCalibrator
{

public:
    Calibrator( const uint64_t gpuIndex,
                const uint frameWidth,
                const uint frameHeight,
                const Transformation &transformation,
                const std::string kernelDirectory,
                const uint iterations ,
                QObject *parent = 0 );

public slots:
    virtual void finishRendering_SLOT( Renderer::CLAbstractRenderer* ) override ;

public:
    /**
     * @brief rendererEngine_
     */
    Renderer::CLRenderer< V , F > rendererEngine_ ;

private:
    /**
     * @brief iterations_
     */
    const uint iterations_;

    /**
     * @brief framesCounter_
     */
    uint framesCounter_ ;
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
