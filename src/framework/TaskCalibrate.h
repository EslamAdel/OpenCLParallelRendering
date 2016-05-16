#ifndef TASKCALIBRATE_H
#define TASKCALIBRATE_H
#include <QThread>
#include <QObject>
#include <QMap>
#include <QList>
#include "Calibrator.h"
#include "Transformation.h"

namespace clparen {
namespace Task {

typedef QMap<uint64_t,double> FrameRates;
class TaskCalibrate : public QThread
{
    Q_OBJECT
public:
    TaskCalibrate(const uint width,
                  const uint height,
                  const uint iterations);

public slots:

    /**
     * @brief finishCalibration_SLOT
     * @param frameRate
     */
    void finishCalibration_SLOT(double frameRate , uint64_t gpuIdx);

signals:

    /**
     * @brief taskCalibrattionFinsished
     * @param FrameRates
     */
    void taskCalibrattionFinsished(double FrameRates );

protected:

    /**
     * @brief run
     */
    void run();

private:

    /**
     * @brief initializeTransformations_
     */
    void initializeTransformations_();
    /**
     * @brief addCalibrator
     */
    void deployGPUs_();

private:
    FrameRates frameRates_;
    QList<calibrator::Calibrator<uchar,float>* > calibrators_;
    Transformation transformations_;
    oclHWDL::Devices listGPUs_;
    const uint frameWidth_;
    const uint frameHeight_;
    const uint iterations_;
    calibrator::Calibrator<uchar , float > * calibrator_ ;

};

}
}
#endif // TASKCALIBRATE_H
