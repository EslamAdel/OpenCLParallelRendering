#include "TaskCalibrate.h"

namespace clparen {
namespace Task {

TaskCalibrate::TaskCalibrate(const uint width , const uint height , const uint iterations) :
    frameWidth_(width),frameHeight_(height),iterations_(iterations)
{
    //Initialize Transformations
    initializeTransformations_();
    //Deploy all GPUs
    //Start Rendering on All GPUs

}

void TaskCalibrate::finishCalibration_SLOT(double frameRate, uint64_t gpuIdx)
{
    frameRates_[gpuIdx] = frameRate;
}

void TaskCalibrate::run()
{
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
        const uint64_t index = idx ;
        calibrator_ = new Calibrator::Calibrator<uchar ,float >(idx ,
                                                                frameWidth_,
                                                                frameHeight_,
                                                                transformations_,
                                                                "/usr/local/share",
                                                                iterations_);
        //        calibrators_.append(cal);
    }

}

}
}
