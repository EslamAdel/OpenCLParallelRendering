#include "ParallelRendering.h"
#include <Logger.h>
#include "Typedefs.hh"
#include <QList>

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
    rotation_.y = INITIAL_VOLUME_ROTATION_Y;
    rotation_.z = INITIAL_VOLUME_ROTATION_Z;


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
    // if device already occupied by rendering node, return.
    if( inUseGPUs_.contains( listGPUs_.at( gpuIndex ))) return;

    RenderingNode *node = new RenderingNode( gpuIndex,
                                             translationAsync_,
                                             rotationAsync_,
                                             volumeDensityAsync_,
                                             brightnessAsync_ );

    inUseGPUs_ << listGPUs_.at( gpuIndex );
    renderingNodes_[ listGPUs_.at( gpuIndex )] = node;

    TaskRender *taskRender = new TaskRender( *node );
    renderingTasks_[ node ] = taskRender;

    TaskCollect *taskCollect = new TaskCollect( *node );
    collectingTasks_[ node ] = taskCollect;

    rendererPool_.setMaxThreadCount( inUseGPUs_.size() );
    collectorPool_.setMaxThreadCount( inUseGPUs_.size() );

    connect( node , SIGNAL(finishedRendering( RenderingNode* )),
             this , SLOT(finishedRendering_SLOT( RenderingNode* )));

    connect( node , SIGNAL(bufferUploaded( RenderingNode* )),
             this , SLOT(bufferUploaded_SLOT( RenderingNode* )));
}

void ParallelRendering::distributeBaseVolume1D()
{

    const int nDevices = inUseGPUs_.size();

    const uint64_t baseXDimension = baseVolume_->getDimensions().x;
    const uint64_t newXDimension = baseXDimension / nDevices ;


    QList< Volume<uchar>* > bricks;

    for( auto i = 0 ; i < nDevices - 1 ; i++ )
    {
        auto *brick = baseVolume_->getBrick( newXDimension*i ,
                                             newXDimension*( i + 1 ) - 1,
                                             0,
                                             baseVolume_->getDimensions().y ,
                                             0,
                                             baseVolume_->getDimensions().z );

        bricks.push_back( brick );
    }

    auto brick = baseVolume_->getBrick( newXDimension*( nDevices - 1 ),
                                        baseVolume_->getDimensions().x ,
                                        0,
                                        baseVolume_->getDimensions().y ,
                                        0,
                                        baseVolume_->getDimensions().z );

    bricks.push_back( brick );

    for( oclHWDL::Device *device  : inUseGPUs_ )
    {
        auto subVolume = bricks.front();
        bricks.pop_front();
        renderingNodes_[ device ]->loadVolume( subVolume );
    }


}

void ParallelRendering::applyTransformation()
{
    syncTransformation();
    RenderingNode *node;
    for( oclHWDL::Device *device : inUseGPUs_ )
    {
        node = renderingNodes_[ device ];

        rendererPool_.start( renderingTasks_[ node ]);
    }
    pendingTransformations_ = false;
}

void ParallelRendering::syncTransformation()
{
    translationAsync_ = translation_;
    rotationAsync_ = rotation_;
    brightnessAsync_ = brightness_ ;
    volumeDensityAsync_ = volumeDensity_;
}

void ParallelRendering::finishedRendering_SLOT(RenderingNode *finishedNode)
{
    collectorPool_.start( collectingTasks_[ finishedNode ]);
}

void ParallelRendering::compositingTaskFinished_SLOT()
{


    if( pendingTransformations_ ) applyTransformation();
}

void ParallelRendering::bufferUploaded_SLOT(RenderingNode *finishedNode)
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
    brightness_ = brightness;
    pendingTransformations_ = true ;
}

void ParallelRendering::updateVolumeDensity_SLOT(float density)
{
    volumeDensity_ = density;
    pendingTransformations_ = true ;
}
