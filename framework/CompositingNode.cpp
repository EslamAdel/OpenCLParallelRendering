#include "CompositingNode.h"
#include "Logger.h"
#include <oclUtils.h>
#include <Volume.h>
#include <Utilities.h>

#include <auxillary/glm/glm.hpp>
#include <auxillary/glm/gtc/matrix_transform.hpp>
#include <auxillary/glm/gtc/type_ptr.hpp>



CompositingNode::CompositingNode( const uint64_t gpuIndex,
                                  const uint framesCount,
                                  const uint frameWidth ,
                                  const uint frameHeight ,
                                  CompositingMode mode)
    : gpuIndex_( gpuIndex ) ,
      framesCount_( framesCount ) ,
      collageFrameDimensions_( frameWidth , frameHeight ) ,
      mode_( mode )
{

    LOG_DEBUG("Constructing CompositingNode:\nFramesCount:%d\n"
              "FrameSize:%dx%d" , framesCount_ ,
              collageFrameDimensions_.x , collageFrameDimensions_.y );

    initializeContext_();
    initializeBuffers_();
    initializeKernel_();

    rewindCollageFrame_DEVICE( CL_TRUE );
}


CompositingNode::~CompositingNode()
{

}

uint64_t CompositingNode::getGPUIndex() const
{
    return gpuIndex_;
}

void CompositingNode::setFrameData_HOST(const uint frameIndex , uint* data )
{
    framesData_[ frameIndex ] = data;

    if( mode_ == CompositingMode::Accumulate )
        frames_[ frameIndex ]->setHostData( data );
    else
        framesArray_->setFrameData( frameIndex , data );
}

void CompositingNode::loadFrameDataToDevice( const uint frameIndex ,
                                             const cl_bool block )
{
    if( mode_ == CompositingMode::Accumulate)
        frames_[ frameIndex ]->writeDeviceData( commandQueue_ , block );
    else
        framesArray_->loadFrameDataToDevice( frameIndex ,
                                             commandQueue_ ,
                                             block );
}

void CompositingNode::accumulateFrame_DEVICE( const uint frameIndex )
{
    if( mode_ != CompositingMode::Accumulate )
        LOG_ERROR("Compositing mode should be Accumulate!");

    const CLFrame32 *currentFrame = frames_[ frameIndex ];

    const cl_mem currentFrameObject = currentFrame->getDeviceData();


    // Assume everything is fine in the begnning
    cl_int clErrorCode = CL_SUCCESS;

    compositingKernel_->setFrame( currentFrameObject );

    const size_t localSize[ ] = { 1 } ;
    const size_t globalSize[ ] = { collageFrameDimensions_.imageSize() };


    clErrorCode = clEnqueueNDRangeKernel( commandQueue_ ,
                                          compositingKernel_->getKernelObject() ,
                                          1 ,
                                          NULL ,
                                          globalSize ,
                                          localSize ,
                                          0 ,
                                          NULL ,
                                          NULL ) ;


    if( clErrorCode != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( clErrorCode );
        LOG_ERROR("OpenCL Error!");
    }

    clFinish( commandQueue_ );
}

void CompositingNode::compositeFrames_DEVICE()
{
    if( mode_ != CompositingMode::PatchCompositing )
        LOG_ERROR("Compositing mode should be PatchCompositng");

    // Assume everything is fine in the begnning
    cl_int clErrorCode = CL_SUCCESS;


    const size_t localSize[ ] = { 1 , 1  };
    const size_t globalSize[ ] = { collageFrameDimensions_.x ,
                                   collageFrameDimensions_.y  } ;

    clErrorCode =
            clEnqueueNDRangeKernel( commandQueue_ ,
                                    compositingKernel_->getKernelObject() ,
                                    2 ,
                                    NULL ,
                                    globalSize ,
                                    localSize ,
                                    0 ,
                                    NULL ,
                                    NULL ) ;
    if( clErrorCode != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( clErrorCode );
        LOG_ERROR("OpenCL Error!");
    }


    clFinish( commandQueue_ );

}

void CompositingNode::rewindCollageFrame_DEVICE(cl_bool blocking)
{
    // Assume everything is fine in the begnning
    cl_int clErrorCode = CL_SUCCESS;


    const size_t localSize[ ] = { 1 };
    const size_t globalSize[ ] = { collageFrameDimensions_.imageSize() } ;

    clErrorCode = clEnqueueNDRangeKernel( commandQueue_  ,
                                          rewindFrameKernel_->getKernelObject()  ,
                                          1 ,
                                          NULL ,
                                          globalSize ,
                                          localSize ,
                                          0 ,
                                          NULL ,
                                          NULL ) ;

    if( clErrorCode != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( clErrorCode );
        LOG_ERROR("OpenCL Error!");
    }

    if( blocking == CL_TRUE ) clFinish( commandQueue_ );
}

void CompositingNode::uploadCollageFromDevice()
{
    collageFrame_->readDeviceData( commandQueue_  , CL_TRUE );
}


QPixmap &CompositingNode::getCollagePixmap()
{
    collageFrame_->getFramePixmap();
}

uint CompositingNode::framesCount() const
{
    return framesCount_ ;
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

    collageFrame_ = new CLFrame32( collageFrameDimensions_ );

    collageFrame_->createDeviceData( context_ );

    uint* frameData ;
    framesData_.push_back( frameData );


    if( mode_ == CompositingMode::Accumulate )
        for( auto i = 0 ; i < framesCount_ ; i++ )
        {
            auto frame = new CLFrame32( collageFrameDimensions_ );
            frame->createDeviceData( context_ );
            frames_.push_back( frame );
        }

    else if( mode_ == CompositingMode::PatchCompositing )
    {
        framesArray_ = new CLImage2DArray32( collageFrameDimensions_.x ,
                                             collageFrameDimensions_.y ,
                                             framesCount_ ) ;
        framesArray_->createDeviceData( context_ );
    }



    LOG_DEBUG("[DONE] Initializing Buffers ...");

}

void CompositingNode::initializeKernel_()
{
    LOG_DEBUG( "Initializing an OpenCL Kernel ... " );

    std::string kernelName ;
    if( mode_ == CompositingMode::Accumulate )
        kernelName = "xray_compositing_accumulate";
    else
        kernelName = "xray_compositing_patch";

    compositingKernel_ = new CLXRayCompositingKernel( context_ , kernelName );
    rewindFrameKernel_ = new CLRewindFrameKernel( context_ );


    //set arguments of rewind_buffer kernel.
    rewindFrameKernel_->setFrame( collageFrame_->getDeviceData() );

    //set arguments of xray_composite kernel.
    compositingKernel_->setCollegeFrame( collageFrame_->getDeviceData() );

    if( mode_ == CompositingMode::PatchCompositing )
        compositingKernel_->setFrame( framesArray_->getDeviceData() );


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

