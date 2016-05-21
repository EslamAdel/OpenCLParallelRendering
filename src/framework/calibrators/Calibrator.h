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
                const Transformation transformation,
                const uint iterations ,
                QObject *parent = 0 );

    /**
     * @brief startCalibration
     */
    double startCalibration();

    /**
     * @brief loadVolume
     * @param volume
     */
    void loadVolume(  const VolumeVariant &volume );


    /**
     * @brief getGPUName
     * @return
     */
    std::string getGPUName() const ;

private:
    Transformation transformation_ ;

    /**
     * @brief rendererEngine_
     */
    Renderer::CLRenderer< V , F > rendererEngine_ ;

    /**
     * @brief iterations_
     */
    const uint iterations_;

};

}
}
#endif // CALIBRATOR_H
