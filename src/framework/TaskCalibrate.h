#ifndef TASKCALIBRATE_H
#define TASKCALIBRATE_H
#include <QThread>
#include <QObject>
#include <QMap>
#include <QList>
#include "Calibrator.h"
#include "Transformation.h"
#include "Volume.h"

namespace clparen {
namespace Task {

//typedef QMap<uint64_t,double> FrameRates;
class TaskCalibrate : public QThread
{
    Q_OBJECT
public:
    TaskCalibrate(const uint width,
                  const uint height,
                  const uint iterations,
                  const uint volumeScale);

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
    void taskCalibrattionFinsished( QVector< double > FrameRates );

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

    void makeDummyVolume_();

private:
//    FrameRates frameRates_;
    QVector< double > frameRates_;
    QList<Calibrator::Calibrator<uchar,float>* > calibrators_;
    Transformation transformations_;
    oclHWDL::Devices listGPUs_;
    const uint frameWidth_;
    const uint frameHeight_;
    const uint iterations_;
    Volume8 * volume_;
    const uint volumeScale_;

};

}
}
#endif // TASKCALIBRATE_H
