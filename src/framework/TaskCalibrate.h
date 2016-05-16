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
    TaskCalibrate();

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
    void addCalibrator_();

private:
    FrameRates frameRates_;
    QList<calibrator::Calibrator<uchar,float> > calibrators_;
    Transformation transformations_;

};

}
}
#endif // TASKCALIBRATE_H
