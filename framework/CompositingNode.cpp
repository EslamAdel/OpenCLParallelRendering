#include "CompositingNode.h"
#include "Logger.h"
#include <oclUtils.h>
#include <Volume.h>
#include <Utilities.h>

#include <auxillary/glm/glm.hpp>
#include <auxillary/glm/gtc/matrix_transform.hpp>
#include <auxillary/glm/gtc/type_ptr.hpp>



CompositingNode::CompositingNode(const uint64_t gpuIndex,
                                 const uint framesCount,
                                 const uint frameWidth ,
                                 const uint frameHeight ,
                                 const std::vector< const Coordinates3D *> framesCenters)
    : gpuIndex_( gpuIndex ) ,
      framesCount_( framesCount ) ,
      collageFrameDimensions_( frameWidth , frameHeight ) ,
      framesCenters_( framesCenters )
{
    compositedFramesCount_ = 0 ;

    if( framesCount != framesCenters.size() )
        LOG_ERROR("Size Mismatch in framesCount and framesCenters count");

    LOG_DEBUG("Constructing CompositingNode:\nFramesCount:%d\n"
              "FrameSize:%dx%d" , framesCount_ ,
              collageFrameDimensions_.x , collageFrameDimensions_.y );

    initializeContext_();
    initializeBuffers_();
    initializeKernel_();

}


CompositingNode::~CompositingNode()
{

}

uint64_t CompositingNode::getGPUIndex() const
{
    return gpuIndex_;
}

CLFrame32 *&CompositingNode::getCLFrameCollage()
{
    return collageFrameReadout_ ;
}

void CompositingNode::setFrameData_HOST( uint* data ,
                                         const uint8_t frameIndex )
{
    frames_[ frameIndex ]->setHostData( data );
}

void CompositingNode::loadFrameToDevice( const uint8_t frameIndex ,
                                         const cl_bool block )
{
    frames_[ frameIndex ]->writeDeviceData( commandQueue_ , block );
}

void CompositingNode::collectFrame( const uint8_t frameIndex ,
                                    cl_command_queue sourceCmdQueue,
                                    const CLFrame32 &sourceFrame ,
                                    const cl_bool block )
{
    //if the two buffers are in same context, copy the buffer directly in Device.


    if( sourceFrame.inSameContext( *frames_[ frameIndex ]))
    {

    }

    else
    {
        frames_[ frameIndex ]->readOtherDeviceData( sourceCmdQueue ,
                                                    sourceFrame ,
                                                    block );

        frames_[ frameIndex ]->writeDeviceData( commandQueue_ ,
                                                block );
    }



}

void CompositingNode::accumulateFrame_DEVICE( const uint frameIndex )
{   
    //if first frame, it is already written to collageFrame, return.
    if( compositedFramesCount_ == 0 )
    {
        //        LOG_DEBUG("Frame[%d] as Collage Buffer", frameIndex );
        //make first loaded frame buffer as collage frame.
        collageFrame_ = frames_[ frameIndex ];
        collageBufferFrameIndex_ = frameIndex ;

        compositingKernel_->
                setCollegeFrame( frames_[ frameIndex ]->getDeviceData() );

        ++compositedFramesCount_;
        return ;
    }


    if( collageBufferFrameIndex_ == frameIndex )
    {
        LOG_ERROR("Accumulating Collage Frame to itself!!");
    }
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
    //    LOG_DEBUG("[DONE] Accumulating Frame[%d]", frameIndex );

    compositedFramesCount_ ++;
}

void CompositingNode::compositeFrames_DEVICE()
{

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

void CompositingNode::loadCollageFromDevice()
{

    //    LOG_DEBUG("Reading CollageFrame[%d]" , collageBufferFrameIndex_ );
    collageFrameReadout_->readOtherDeviceData( commandQueue_ ,
                                               *collageFrame_ ,
                                               CL_TRUE );
    //    LOG_DEBUG("[DONE] Reading CollageFrame[%d]" , collageBufferFrameIndex_ );

    compositedFramesCount_ = 0 ;
}

uint CompositingNode::framesCount() const
{
    return framesCount_ ;
}

uint8_t CompositingNode::getCompositedFramesCount() const
{
    return compositedFramesCount_ ;
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

    collageFrameReadout_ = new CLFrame32( collageFrameDimensions_ );

    for( auto i = 0 ; i < framesCount_ ; i++ )
    {
        auto frame = new CLFrame32( collageFrameDimensions_ );
        frame->createDeviceData( context_ );
        frames_.push_back( frame );
    }


    //set CollageFrame to the first frame
    collageFrame_ = frames_[ 0 ] ;

    LOG_DEBUG("[DONE] Initializing Buffers ...");

}

void CompositingNode::initializeKernel_()
{
    LOG_DEBUG( "Initializing an OpenCL Kernel ... " );

    compositingKernel_ =
            new CLXRayCompositingKernel( context_ ,
                                         "xray_compositing_accumulate" );

    //set arguments of xray_composite kernel.
    compositingKernel_->setCollegeFrame( collageFrame_->getDeviceData() );


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
