#include "TaskCalibrate.h"

namespace clparen {
namespace Calibrator {
namespace Task {

TaskCalibrate::TaskCalibrate( const Dimensions2D frameDimensions,
                              const Dimensions3D volumeDimensions,
                              const uint iterations )
    : frameDimensions_( frameDimensions ) ,
      volumeDimensions_( volumeDimensions ) ,
      iterations_( iterations )
{

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

    // Scale
    transformations_.scale.x = 1.0;
    transformations_.scale.y = 1.0;
    transformations_.scale.z = 1.0;
}


void TaskCalibrate::run()
{
    //Initialize Transformations
    initializeTransformations_();
    //Deploy all GPUs
    deployGPUs_();
    //Generate dummy volume
    generateDummyVolume_();


    for( const uint64_t gpuIndex : calibrators_.keys( ))
        renderingTimes_[ gpuIndex ] =
                calibrators_[ gpuIndex ]->startCalibration();

    emit calibrationFinsished_SIGNAL( renderingTimes_ );

}



void TaskCalibrate::deployGPUs_()
{
    oclHWDL::Hardware clHardware;
    listGPUs_ = clHardware.getListGPUs();
    for( uint64_t idx = 0 ; idx < listGPUs_.size() ; idx++ )
    {

        Calibrator<uchar ,float > *calibrator =
                new Calibrator<uchar ,float >( idx ,
                                               frameDimensions_.x ,
                                               frameDimensions_.y ,
                                               transformations_,
                                               "/usr/local/share",
                                               iterations_ );

        calibrators_[ idx ] = calibrator;
    }

}

void TaskCalibrate::generateDummyVolume_()
{
    Coordinates3D volumeCoordinates( 0.5 , 0.5 , 0.5 );
    Coordinates3D unitCubeCenter( 0.5 , 0.5 , 0.5 );
    Coordinates3D unitCubeScaleFactors( 1.f , 1.f , 1.f );

    const uint64_t volumeSize = volumeDimensions_.volumeSize() ;

    uchar* data =  new uchar[ volumeSize ];

    int seed = time(NULL);
    srand( seed );


    for( uint64_t i = 0 ; i < volumeSize ; i++ )
        data[ i ] = rand() % 255 ;


    volume_.reset( new Volume8( volumeCoordinates,
                                volumeDimensions_ ,
                                unitCubeCenter ,
                                unitCubeScaleFactors ,
                                data ));

    VolumeVariant volume = VolumeVariant::fromValue( volume_.data( ));

    for( Calibrator< uchar , float > *calibrator : calibrators_ )
        calibrator->loadVolume(volume);

}

}
}
}
