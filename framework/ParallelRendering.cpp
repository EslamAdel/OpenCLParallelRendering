#include "ParallelRendering.h"
#include <Logger.h>

#define VOLUME_PREFIX "/projects/volume-datasets/foot/foot"
#define INITIAL_VOLUME_CENTER_X 0.0
#define INITIAL_VOLUME_CENTER_Y 0.0
#define INITIAL_VOLUME_CENTER_Z -4.0
#define INITIAL_VOLUME_ROTATION_X 0.0
#define INITIAL_VOLUME_ROTATION_Y 0.0
#define INITIAL_VOLUME_ROTATION_Z 0.0

ParallelRendering::ParallelRendering( Volume<uchar> *volume )
{
    loadBaseVolume( volume );
    listGPUs_ = clHardware_.getListGPUs();

    // Translation
    translation_.x = INITIAL_VOLUME_CENTER_X;
    translation_.y = INITIAL_VOLUME_CENTER_X;
    translation_.z = INITIAL_VOLUME_CENTER_Z;

    // Rotation
    rotation_.x = INITIAL_VOLUME_ROTATION_X;
    rotation_.x = INITIAL_VOLUME_ROTATION_Y;
    rotation_.x = INITIAL_VOLUME_ROTATION_Z;
}

void ParallelRendering::loadBaseVolume( Volume<uchar> *volume )
{
    LOG_INFO( "Loading Base volume" );

    baseVolume_ = volume;

}

void ParallelRendering::discoverAllNodes()
{


    for(uint64_t gpuIndex = 0 ;  gpuIndex < listGPUs_.size() ; gpuIndex++ )
    {
        addRenderingNode( gpuIndex );
    }

}

void ParallelRendering::addRenderingNode( const uint64_t gpuIndex)
{
    // if out of order, return. In case used from the main().
    if( inUseGPUs_.contains( listGPUs_.at( gpuIndex ))) return;

    RenderingNode *node = new RenderingNode( gpuIndex );
    inUseGPUs_ << listGPUs_.at( gpuIndex );

    TaskRender *taskRender = new TaskRender( *node );
    renderingTasks_[ node ] = taskRender;

    TaskCollect *taskCollect = new TaskCollect( *node );
    collectingTasks_[ node ] = taskCollect;

    rendererPool_.setMaxThreadCount( inUseGPUs_.size() );
    collectorPool_.setMaxThreadCount( inUseGPUs_.size() );

}

void ParallelRendering::applyTransformation()
{

}

void ParallelRendering::renderingTaskFinished_SLOT(RenderingNode *finishedNode)
{

}

void ParallelRendering::compositingTaskFinished_SLOT()
{


}

void ParallelRendering::imageUploaded_SLOT(RenderingNode *finishedNode)
{

}

void ParallelRendering::updateRotationX_SLOT(int angle)
{
    rotation_.x = angle ;
    pendingTransformations_ = true ;
}

void ParallelRendering::updateRotationY_SLOT(int angle)
{
    rotation_.y = angle ;
    pendingTransformations_ = true ;
}

void ParallelRendering::updateRotationZ_SLOT(int angle)
{
    rotation_.z = angle ;
    pendingTransformations_ = true ;
}

void ParallelRendering::updateTranslationX_SLOT(int distance)
{
    translation_.x = distance;
    pendingTransformations_ = true ;
}

void ParallelRendering::updateTranslationY_SLOT(int distance)
{
    translation_.x = distance;
    pendingTransformations_ = true ;
}

void ParallelRendering::updateImageBrightness_SLOT(float brightness)
{
    imageBrightness_ = brightness;
    pendingTransformations_ = true ;
}

void ParallelRendering::updateVolumeDensity_SLOT(float density)
{
    volumeDensity_ = density;
    pendingTransformations_ = true ;
}
