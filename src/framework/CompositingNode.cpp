#include "CompositingNode.h"
#include "Logger.h"
#include <oclUtils.h>
#include <Volume.h>
#include <Utilities.h>

#include <auxillary/glm/glm.hpp>
#include <auxillary/glm/gtc/matrix_transform.hpp>
#include <auxillary/glm/gtc/type_ptr.hpp>



CompositingNode::CompositingNode(const uint64_t gpuIndex,
                                 const uint frameWidth ,
                                 const uint frameHeight )
    : gpuIndex_( gpuIndex ) ,
      collageFrameDimensions_( frameWidth , frameHeight )
{
    compositedFramesCount_ = 0 ;
    framesCount_ = 0 ;


    initializeContext_();
    initializeBuffers_();
    initializeKernel_();

}


CompositingNode::~CompositingNode()
{

}

void CompositingNode::allocateFrame( RenderingNode *renderingNode )
{

    CLFrame32 *frame =
            new CLFrame32( renderingNode->getCLFrame()->getFrameDimensions( ));

    frame->createDeviceData( context_ );

    frames_[ renderingNode ] = frame ;

    framesCount_++ ;
}

uint64_t CompositingNode::getGPUIndex() const
{
    return gpuIndex_;
}

CLFrame32 *&CompositingNode::getCLFrameCollage()
{
    return collageFrameReadout_ ;
}


void CompositingNode::collectFrame( RenderingNode *renderingNode ,
                                    const cl_bool block )
{
    //if the two buffers are in same context, copy the buffer directly in Device.


    if( renderingNode->getContext() ==
        frames_[ renderingNode ]->getContext( ))
    {
        //TODO: Create the CompositingNode at the same context of the
        //RenderingNode that attached to the same GPU.


    }
    else
    {

        frames_[ renderingNode ]->
                readOtherDeviceData( renderingNode->getCommandQueue() ,
                                     *renderingNode->getCLFrame() ,
                                     block );

        frames_[ renderingNode ]->
                writeDeviceData( commandQueue_ ,
                                 block );
    }

}

void CompositingNode::accumulateFrame_DEVICE( RenderingNode *renderingNode )
{   
    //if first frame, it is already written to collageFrame, return.
    if( compositedFramesCount_ == 0 )
    {
        //        LOG_DEBUG("Frame[%d] as Collage Buffer", frameIndex );
        //make first loaded frame buffer as collage frame.
        collageFrame_ = frames_[ renderingNode ];

        compositingKernel_->
                setCollageFrame( collageFrame_->getDeviceData( ));

        ++compositedFramesCount_;
        return ;
    }


    const CLFrame32 *currentFrame = frames_[ renderingNode ];


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


    LOG_DEBUG("[DONE] Initializing Buffers ...");

}

void CompositingNode::initializeKernel_()
{
    LOG_DEBUG( "Initializing an OpenCL Kernel ... " );

    compositingKernel_ =
            new CLXRayCompositingKernel( context_ ,
                                         "xray_compositing_accumulate" );

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
