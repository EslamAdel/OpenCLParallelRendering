#ifndef ABSTRACTCALIBRATOR_H
#define ABSTRACTCALIBRATOR_H

#include <QObject>
#include "CLAbstractRenderer.h"

namespace clparen {
namespace Calibrator {

class AbstractCalibrator : public QObject
{
    Q_OBJECT

public:
    AbstractCalibrator( QObject *parent = 0 );

signals:
    void finishedCalibration_SIGNAL( double meanTime , uint64_t gpuIdx );


};


}
}


#endif // ABSTRACTCALIBRATOR_H
