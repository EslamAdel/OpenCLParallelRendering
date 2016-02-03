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

ParallelRendering::ParallelRendering( Volume<uchar> *volume ,
                                      const uint frameWidth ,
                                      const uint frameHeight )
    : frameWidth_( frameWidth ),
      frameHeight_( frameHeight )
{
    renderingNodesReady_ = false ;
    compositingNodeSpecified_ = false ;

    loadBaseVolume( volume );
    listGPUs_ = clHardware_.getListGPUs();

    machineGPUsCount_ = listGPUs_.size();

    // Translation
    translation_.x = INITIAL_VOLUME_CENTER_X;
    translation_.y = INITIAL_VOLUME_CENTER_X;
    translation_.z = INITIAL_VOLUME_CENTER_Z;

    // Rotation
    rotation_.x = INITIAL_VOLUME_ROTATION_X;
    rotation_.y = INITIAL_VOLUME_ROTATION_Y;
    rotation_.z = INITIAL_VOLUME_ROTATION_Z;

    compositorPool_.setMaxThreadCount( 1 );


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

    // pass transformations by reference.
    // translationAsync_, rotationAsync_, volumeDensityAsync_, brightnessAsync_
    // will be accessed by multithreads. So,
    // they should not be modified during the life of the threads.
    RenderingNode *node = new RenderingNode( gpuIndex,
                                             frameWidth_ ,
                                             frameHeight_ ,
                                             translationAsync_,
                                             rotationAsync_,
                                             volumeDensityAsync_,
                                             brightnessAsync_ );

    inUseGPUs_ << listGPUs_.at( gpuIndex );
    renderingNodes_[ listGPUs_.at( gpuIndex )] = node;

    TaskRender *taskRender = new TaskRender( *node );
    renderingTasks_[ node ] = taskRender;

    rendererPool_.setMaxThreadCount( inUseGPUs_.size( ));
    collectorPool_.setMaxThreadCount( inUseGPUs_.size( ));

    connect( node , SIGNAL( finishedRendering( RenderingNode* )),
             this , SLOT( finishedRendering_SLOT( RenderingNode* )));

//    connect( node , SIGNAL( bufferUploaded( RenderingNode* )),
//             this , SLOT( bufferUploaded_SLOT( RenderingNode* )));
}

void ParallelRendering::addCompositingNode( const uint64_t gpuIndex )
{

    if( gpuIndex >= listGPUs_.size() )
        LOG_ERROR("Choose GPU index from [0-%d]", listGPUs_.size() - 1 );

    if( compositingNodeSpecified_ )
    {
        LOG_WARNING("Compositing Node already attached to GPU <%d>" ,
                    compositingNode_->getGPUIndex( ));
        return ;
    }

    // add compositingNode_ that will manage compositing rendered frames.
    compositingNode_ = new CompositingNode( gpuIndex ,
                                            inUseGPUs_.size() ,
                                            frameWidth_ ,
                                            frameHeight_ );



    // for each rendering task, a collecting task and compositing task will follow.
    uint frameIndex = 0 ;
    for( oclHWDL::Device *rendererDevice : inUseGPUs_ )
    {

        RenderingNode *node = renderingNodes_[ rendererDevice ];

        //now add collectingTasks that transfer data from Device_x, where rendering performed,
        // to Host then to Device_y where compositing performed.
        auto collectingTask = new TaskCollect( node , compositingNode_ ,
                                               frameIndex );
        collectingTasks_[ node ] = collectingTask ;


        // add compositing task, that will execute concurrently to accumulate
        // a frame to the collageFrame.
        auto compositingTask =
                new TaskComposite( compositingNode_ , frameIndex );
        compositingTasks_[ node ] = compositingTask ;

        connect( collectingTask ,
                 SIGNAL( frameLoadedToDevice_SIGNAL( RenderingNode* )) ,
                 this , SLOT( frameLoadedToDevice_SLOT( RenderingNode* )));

        connect( compositingTask , SIGNAL(compositingFinished_SIGNAL( )) ,
                 this , SLOT(compositingFinished_SLOT( )));

        frameIndex++ ;
    }


    compositingNodeSpecified_ = true ;
}

void ParallelRendering::distributeBaseVolume1D()
{
    LOG_DEBUG("Distributing Volume");

    if( baseVolume_ == nullptr )
        LOG_ERROR( "No Base Volume to distribute!" );

    const int nDevices = inUseGPUs_.size();

    if( nDevices == 0 )
        LOG_ERROR( "No deployed devices to distribute volume!");

    const uint64_t baseXDimension = baseVolume_->getDimensions().x;
    const uint64_t newXDimension =  baseXDimension / nDevices  ;



    for( auto i = 0 ; i < nDevices - 1 ; i++ )
    {
        auto *brick = baseVolume_->getBrick( newXDimension*i ,
                                             newXDimension*( i + 1 ) - 1,
                                             0,
                                             baseVolume_->getDimensions().y ,
                                             0,
                                             baseVolume_->getDimensions().z );

        bricks_.push_back( brick );
    }

    auto brick = baseVolume_->getBrick( newXDimension*( nDevices - 1 ),
                                        baseVolume_->getDimensions().x ,
                                        0,
                                        baseVolume_->getDimensions().y ,
                                        0,
                                        baseVolume_->getDimensions().z );

    bricks_.push_back( brick );
    int i = 0;

    for( oclHWDL::Device *device  : inUseGPUs_ )
    {
        LOG_DEBUG( "Loading subVolume to device" );
        auto subVolume = bricks_[ i++ ];
        renderingNodes_[ device ]->loadVolume( subVolume );
        LOG_DEBUG( "[DONE] Loading subVolume to GPU <%d>",
                   renderingNodes_[ device ]->gpuIndex( ));
    }

    // space optimizations:
    // -delete the base volume after distribution
    // -use of memory mapping
}

void ParallelRendering::startRendering()
{
    activeRenderingNodes_ = inUseGPUs_.size();

    LOG_INFO("Triggering Rendering Nodes");
    //force trigger rebdering
    applyTransformation();
    LOG_INFO("[DONE] Triggering Rendering Nodes");
}

void ParallelRendering::applyTransformation()
{
    readyPixmapsCount_ = 0 ;
    syncTransformation();
    RenderingNode *node;
    for( oclHWDL::Device *device : inUseGPUs_ )
    {
        node = renderingNodes_[ device ];
        //LOG_DEBUG("Triggering rendering node <%d>", node->gpuIndex() );
        rendererPool_.start( renderingTasks_[ node ]);

    }
    pendingTransformations_ = false;
    renderingNodesReady_ = false;
}

void ParallelRendering::syncTransformation()
{
    // ***Async objects modified when no active rendering threads
    translationAsync_ = translation_;
    rotationAsync_ = rotation_;
    brightnessAsync_ = brightness_ ;
    volumeDensityAsync_ = volumeDensity_;
}

RenderingNode &ParallelRendering::getRenderingNode(const uint64_t gpuIndex)
{
    if(!( inUseGPUs_.contains( listGPUs_.at( gpuIndex ))))
    {
        LOG_ERROR("No such rendering node!");
    }

    return *renderingNodes_[ listGPUs_.at( gpuIndex )];

}

uint8_t ParallelRendering::machineGPUsCount() const
{
    return machineGPUsCount_;
}

uint8_t ParallelRendering::activeRenderingNodesCount() const
{
    return activeRenderingNodes_;
}

void ParallelRendering::finishedRendering_SLOT(RenderingNode *finishedNode)
{
    LOG_DEBUG( "[DONE RENDERING] on GPU <%d> " , finishedNode->gpuIndex() );

    collectorPool_.start( collectingTasks_[ finishedNode ]);
}

void ParallelRendering::compositingFinished_SLOT()
{

    static const uint framesCount = compositingNode_->framesCount();
    static uint framesComposited = 0 ;
    ++framesComposited;
    LOG_DEBUG("Composited Frames:%d/%d" , framesComposited , framesCount );

    if( framesComposited  == framesCount )
    {
        compositingNode_->uploadCollageFromDevice();
        compositingNode_->rewindCollageFrame_DEVICE( CL_FALSE );
        QPixmap &finalFrame = compositingNode_->getCollagePixmap();

        emit this->finalFrameReady_SIGNAL( finalFrame );

        framesComposited = 0 ;
        if( pendingTransformations_ ) applyTransformation();
        renderingNodesReady_ = true ;
    }

}

void ParallelRendering::frameLoadedToDevice_SLOT( RenderingNode *node )
{
    //    LOG_DEBUG( "GPU <%d> frame ready to display" , finishedNode->gpuIndex() );
    //    if( ++readyPixmapsCount_ == activeRenderingNodes_ )
    //    {
    //        emit this->framesReady_SIGNAL();

    //    }

    LOG_DEBUG( "[DONE TRANSFER] from GPU <%d>" , node->gpuIndex() );

    compositorPool_.start( compositingTasks_[ node ]);
}

void ParallelRendering::updateRotationX_SLOT(int angle)
{
    rotation_.x = angle ;
    if( renderingNodesReady_ ) applyTransformation();
    pendingTransformations_ = true ;
}

void ParallelRendering::updateRotationY_SLOT(int angle)
{
    rotation_.y = angle ;
    if( renderingNodesReady_ ) applyTransformation();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateRotationZ_SLOT(int angle)
{
    rotation_.z = angle ;
    if( renderingNodesReady_ ) applyTransformation();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateTranslationX_SLOT(int distance)
{
    translation_.x = distance;
    if( renderingNodesReady_ ) applyTransformation();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateTranslationY_SLOT(int distance)
{
    translation_.y = distance;
    if( renderingNodesReady_ ) applyTransformation();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateImageBrightness_SLOT(float brightness)
{
    brightness_ = brightness;
    if( renderingNodesReady_ ) applyTransformation();
    else pendingTransformations_ = true ;
}

void ParallelRendering::updateVolumeDensity_SLOT(float density)
{
    volumeDensity_ = density;
    if( renderingNodesReady_ ) applyTransformation();
    else pendingTransformations_ = true ;
}
