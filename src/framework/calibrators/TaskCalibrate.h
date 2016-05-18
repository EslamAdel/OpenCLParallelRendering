#ifndef TASKCALIBRATE_H
#define TASKCALIBRATE_H
#include <QRunnable>
#include <QObject>
#include <QMap>
#include <QList>
#include "Calibrator.h"
#include "Transformation.h"
#include "Volume.h"
#include <QScopedPointer>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>

namespace clparen {
namespace Calibrator {
namespace Task {

//typedef QMap<uint64_t,double> FrameRates;
class TaskCalibrate : public QObject , public QRunnable
{
    Q_OBJECT
public:
    TaskCalibrate( const Dimensions2D frameDimensions ,
                   const Dimensions3D volumeDimensions ,
                   const uint iterations );

signals:

    /**
     * @brief taskCalibrattionFinsished
     * @param FrameRates
     */
    void calibrationFinsished_SIGNAL( QMap< uint64_t , double > renderingTimes );

protected:

    /**
     * @brief run
     */
    void run();

private:

    /**
     * @brief initializeTransformations_
     */
    void initializeTransformations_( );
    /**
     * @brief addCalibrator
     */
    void deployGPUs_( );

    /**
     * @brief makeDummyVolume_
     */
    void generateDummyVolume_( );

private:
    /**
     * @brief frameRates_
     */
    QMap< uint64_t , double > renderingTimes_ ;


    /**
     * @brief calibrators_
     */
    QMap< uint64_t ,  Calibrator< uchar , float >* > calibrators_;

    /**
     * @brief listGPUs_
     */
    oclHWDL::Devices listGPUs_;

    /**
     * @brief transformations_
     */
    Transformation transformations_ ;

    /**
     * @brief frameDimensions_
     */
    const Dimensions2D frameDimensions_ ;

    /**
     * @brief volumeDimensions_
     */
    const Dimensions3D volumeDimensions_ ;


    /**
     * @brief iterations_
     */
    const uint iterations_;

    /**
     * @brief volume_
     */
    QScopedPointer< Volume8 > volume_;

};

}
}
}

#endif // TASKCALIBRATE_H
