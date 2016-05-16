#include "TaskCalibrate.h"

namespace clparen {
namespace Task {

TaskCalibrate::TaskCalibrate()
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

void TaskCalibrate::addCalibrator_()
{

}

}
}
