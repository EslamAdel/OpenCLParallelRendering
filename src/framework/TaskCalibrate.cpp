#include "TaskCalibrate.h"

namespace clparen {
namespace Task {

TaskCalibrate::TaskCalibrate(const uint width , const uint height , const uint iterations, const uint volumeScale) :
    frameWidth_(width),frameHeight_(height),iterations_(iterations),volumeScale_(volumeScale)
{
    //Initialize Transformations
    initializeTransformations_();
    //Deploy all GPUs
    deployGPUs_();
    //Generate dummy volume
    makeDummyVolume_();
}

void TaskCalibrate::finishCalibration_SLOT(double frameRate, uint64_t gpuIdx)
{
    static int it = 0 ;
    frameRates_.insert(gpuIdx,frameRate);
    if(++it == listGPUs_.size())
        emit this->taskCalibrattionFinsished(frameRates_);
}

void TaskCalibrate::run()
{
    for(int i = 0 ; i < calibrators_.size(); i++)
    {
        calibrators_.at(i)->rendererEngine_.applyTransformation();
    }
    exec();
}

void TaskCalibrate::initializeTransformations_()
{
    // Translation
    transformations_.translation.x = 0.0;
    transformations_.translation.y = 0.0;
    transformations_.translation.z = 0.0;

    // Rotation
    transformations_.rotation.x = 0.0;
    transformations_.rotation.y = 0.0;
    transformations_.rotation.z = 0.0;

    //Scale
    transformations_.scale.x = 1.0;
    transformations_.scale.y = 1.0;
    transformations_.scale.z = 1.0;
}

void TaskCalibrate::deployGPUs_()
{
    oclHWDL::Hardware clHardware;
    listGPUs_ = clHardware.getListGPUs();
    for(uint64_t idx = 0 ; idx < listGPUs_.size() ; idx++)
    {

        auto caliberator = new Calibrator::Calibrator<uchar ,float >(idx ,
                                                                     frameWidth_,
                                                                     frameHeight_,
                                                                     transformations_,
                                                                     "/usr/local/share",
                                                                     iterations_);

        connect(caliberator,SIGNAL(finishCalibration_SIGNAL(double,uint64_t)),this,
                SLOT(finishCalibration_SLOT(double,uint64_t)));
        calibrators_.append(caliberator);
    }

}

void TaskCalibrate::makeDummyVolume_()
{
    Coordinates3D volumeCoordinates(volumeScale_/2,
                                    volumeScale_/2,
                                    volumeScale_/2);
    Dimensions3D  volumeDimensions(volumeScale_,
                                   volumeScale_,
                                   volumeScale_);
    Coordinates3D unitCubeCenter( 0.5 , 0.5 , 0.5 );
    Coordinates3D unitCubeScaleFactors( 1.f , 1.f , 1.f );
    uchar* data = (uchar*) malloc(volumeDimensions.volumeSize()*sizeof(uchar));
    volume_ = new Volume8(volumeCoordinates,
                         volumeDimensions,
                          unitCubeCenter,
                          unitCubeScaleFactors,
                          data);
    VolumeVariant volume = VolumeVariant::fromValue(volume_);
    for(int i = 0 ; i < calibrators_.size(); i++)
    {
        calibrators_.at(i)->rendererEngine_.loadVolume(volume);
    }
}

}
}
