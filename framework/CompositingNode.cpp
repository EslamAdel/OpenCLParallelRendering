#include "CompositingNode.h"
#include "Logger.h"
#include <oclUtils.h>
#include <Volume.h>
#include <Utilities.h>

#include <auxillary/glm/glm.hpp>
#include <auxillary/glm/gtc/matrix_transform.hpp>
#include <auxillary/glm/gtc/type_ptr.hpp>



CompositingNode::CompositingNode(const uint64_t gpuIndex,
                                 const std::vector<Dimensions2D *> framesDimenstions,
                                 const std::vector<Coordinates3D *> framesCenters,
                                 const uint framesCount,
                                 const Dimensions2D collageFrameDimensions)
    : gpuIndex_( gpuIndex ) ,
      framesDimensions_( framesDimenstions ),
      framesCenters_( framesCenters ),
      framesCount_( framesCount ) ,
      collageFrameDimensions_( collageFrameDimensions )
{
    if( framesCount_ != framesDimensions_.size() ||
            framesCount_!= framesCenters_.size()  )
        LOG_ERROR("Containers size mismatch!");

    initializeContext_();
    initializeBuffers_();
    initializeKernel_();

    rewindCollageFrame();
}

CompositingNode::~CompositingNode()
{

}

uint64_t CompositingNode::getGPUIndex() const
{
    return gpuIndex_;
}

uint *&CompositingNode::frameData(const uint frameIndex)
{
    return framesData_[ frameIndex ];
}

void CompositingNode::uploadFrameData(const uint frameIndex)
{
    frames_[ frameIndex ]->writeDeviceImage( commandQueue_ );
}

void CompositingNode::accumulateFrame(const uint frameIndex)
{
    const CLFrame32 *currentFrame = frames_[ frameIndex ];

    const cl_mem currentFrameObject = currentFrame->getDeviceImage();

    const Dimensions2D &currentDimensions = *framesDimensions_[ frameIndex ];

    const Coordinates3D &currentCenter = *framesCenters_[ frameIndex ];

    // Assume everything is fine in the begnning
    static cl_int clErrorCode = CL_SUCCESS;

    compositingKernel_->setFrame( currentFrameObject );
    compositingKernel_->setFrameWidth( currentDimensions.x );
    compositingKernel_->setFrameHeight( currentDimensions.y );
    compositingKernel_->setFrameCenterX( currentCenter.x );
    compositingKernel_->setFrameCenterY( currentCenter.y );

    static const size_t localSize[ ] = { 1 , 1 } ;
    const size_t globalSize[ ] = { currentDimensions.x , currentDimensions.y };


    clErrorCode = clEnqueueNDRangeKernel( commandQueue_ ,
                                          compositingKernel_->getKernelObject() ,
                                          2 ,
                                          NULL ,
                                          globalSize ,
                                          localSize ,
                                          0 ,
                                          NULL ,
                                          NULL ) ;


    if( clErrorCode != CL_SUCCESS )
        oclHWDL::Error::checkCLError( clErrorCode );
}

//cl_platform_id CompositingNode::getPlatformId() const
//{
//    return platform_;
//}

//cl_device_id CompositingNode::getDeviceId() const
//{
//    return device_;
//}

//cl_context CompositingNode::getContext() const
//{
//    return context_;
//}

//cl_command_queue CompositingNode::getCommandQueue() const
//{
//    return commandQueue_;
//}

//cl_kernel CompositingNode::getKernel() const
//{
//    return kernel_;
//}

void CompositingNode::rewindCollageFrame()
{
    // Assume everything is fine in the begnning
    static cl_int clErrorCode = CL_SUCCESS;


    static const size_t localSize[ ] = { 1 , 1 };
    static const size_t globalSize[ ] = { collageFrameDimensions_.x ,
                                          collageFrameDimensions_.y } ;

    clErrorCode = clEnqueueNDRangeKernel( commandQueue_  ,
                                          rewindFrameKernel_->getKernelObject()  ,
                                          2 ,
                                          NULL ,
                                          globalSize ,
                                          localSize ,
                                          0 ,
                                          NULL ,
                                          NULL ) ;

    if( clErrorCode != CL_SUCCESS )
        oclHWDL::Error::checkCLError( clErrorCode );

}

void CompositingNode::selectGPU_()
{
    // Scan the hardware
    oclHWDL::Hardware* clHardware = new oclHWDL::Hardware();

    // Get a list of all the GPUs that are connected to the system
    const oclHWDL::Devices listGPUs = clHardware->getListGPUs();

    // Select the GPU that will be used for running the kernel
    oclHWDL::Device* selectedGPU = listGPUs.at( gpuIndex_ );
    device_ = selectedGPU->getId();

    // Get the platform that corresponds to the GPU
    platform_ = selectedGPU->getPlatform()->getPlatformId();

    // Create the OpenCL context
    oclHWDL::Context* clContext =
            new oclHWDL::Context(selectedGPU, oclHWDL::BASIC_CL_CONTEXT);
    context_ = clContext->getContext();
}

void CompositingNode::initializeContext_()
{
    LOG_DEBUG( "Initializing an OpenCL context ... " );

    selectGPU_();
    createCommandQueue_();

    LOG_DEBUG( "[DONE] Initializing an OpenCL context ... " );
}

void CompositingNode::initializeBuffers_()
{

    LOG_DEBUG("Initializing Buffers ...");

    Coordinates3D *collageCenter = new Coordinates3D( 0.f , 0.f , 0.f );

    uint* collageData = new uint[ collageFrameDimensions_.imageSize() ];
    collageFrame_ =
            new CLFrame32( collageData , collageFrameDimensions_,  *collageCenter );

    collageFrame_->createDeviceImage( context_ );

    for( auto i=0 ; i < framesCount_ ; i++ )
    {
        uint* frameData ;
        framesData_.push_back( frameData );

        CLFrame32 *frame = new CLFrame32( frameData ,
                                          *framesDimensions_[i] ,
                                          *framesCenters_[i] );
        frame->createDeviceImage( context_ );
        frames_.push_back( frame );

    }


    LOG_DEBUG("[DONE] Initializing Buffers ...");

}

void CompositingNode::initializeKernel_()
{
    LOG_DEBUG( "Initializing an OpenCL Kernel ... " );


    compositingKernel_ = new CLXRayCompositingKernel( context_ );
    rewindFrameKernel_ = new CLRewindFrameKernel( context_ );


    //set arguments of rewind_image2d kernel.
    rewindFrameKernel_->setFrame( collageFrame_->getDeviceImage() );
    rewindFrameKernel_->setFrameWidth( collageFrameDimensions_.x );
    rewindFrameKernel_->setFrameHeight( collageFrameDimensions_.y );

    //set arguments of xray_composite kernel.
    compositingKernel_->setCollegeFrame( collageFrame_->getDeviceImage() );
    compositingKernel_->setCollageWidth( collageFrameDimensions_.x );
    compositingKernel_->setCollageHeight( collageFrameDimensions_.y );


    LOG_DEBUG( "[DONE] Initializing an OpenCL Kernel ... " );
}

void CompositingNode::createCommandQueue_()
{
    cl_int clErrorCode = CL_SUCCESS;
    commandQueue_ = clCreateCommandQueue( context_,
                                          device_,
                                          0, &clErrorCode );

    if( clErrorCode != CL_SUCCESS )
        oclHWDL::Error::checkCLError( clErrorCode );
}

