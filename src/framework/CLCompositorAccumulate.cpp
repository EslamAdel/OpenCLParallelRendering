#include "CLCompositorAccumulate.h"

CLCompositorAccumulate::CLCompositorAccumulate( const uint64_t gpuIndex ,
                                                const uint frameWidth ,
                                                const uint frameHeight )
    : CLAbstractCompositor( gpuIndex ) ,
      frameDimensions_( Dimensions2D( frameWidth , frameHeight ))
{
    compositedFramesCount_ = 0 ;
    framesCount_ = 0 ;

    initializeKernel_();
    initializeBuffers_();
}

void CLCompositorAccumulate::allocateFrame( CLRenderer *renderer )
{

    if( renderers_.contains( renderer ))
        return ;

    renderer->setFrameIndex( renderers_.size( ));

    renderers_ << renderer ;

    CLFrame32 *frame =
            new CLFrame32( renderer->getCLFrame()->getFrameDimensions( ));

    frame->createDeviceData( context_ );

    frames_[ renderer ] = frame ;

    framesCount_++ ;

}

void CLCompositorAccumulate::collectFrame( CLRenderer *renderer ,
                                           const cl_bool block )
{
#ifdef BENCHMARKING
    //    direct copy frame from rendering device to the host pointer
    //    of the compositing frame.
    //    more effiecent.
    frames_[ renderer ]->
            readOtherDeviceData( renderer->getCommandQueue() ,
                                 *renderer->getCLFrame() ,
                                 block );

#else

    frames_[ renderer ]->
            copyHostData( renderer->getCLFrame()->getHostData( ));

    renderer->getCLFrame()->
            readDeviceData( renderer->getCommandQueue() ,
                            block );

#endif
    frames_[ renderer ]->
            writeDeviceData( commandQueue_ ,
                             block );


    loadedFrames_.enqueue( frames_[ renderer ] );
}

void CLCompositorAccumulate::composite( )
{
    if( ++compositedFramesCount_ == framesCount_ )
        readOutReady_ = true ;

    //if first frame, it is already written to collageFrame, return.
    if( compositedFramesCount_ == 1 )
    {
        //        LOG_DEBUG("Frame[%d] as Collage Buffer", frameIndex );
        //make first loaded frame buffer as collage frame.
        finalFrame_ = loadedFrames_.dequeue() ;

        compositingKernel_->
                setCollageFrame( finalFrame_->getDeviceData( ));
        return ;
    }

    const cl_mem currentFrameObject = loadedFrames_.dequeue()->getDeviceData();

    // Assume everything is fine in the begnning
    cl_int clErrorCode = CL_SUCCESS;

    compositingKernel_->setFrame( currentFrameObject );

    const size_t localSize[ ] = { 1 } ;
    const size_t globalSize[ ] = { frameDimensions_.imageSize() };



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
}

void CLCompositorAccumulate::loadFinalFrame()
{
    //    LOG_DEBUG("Reading CollageFrame[%d]" , collageBufferFrameIndex_ );
    finalFrameReadout_->readOtherDeviceData( commandQueue_ ,
                                             *finalFrame_ ,
                                             CL_TRUE );
    //    LOG_DEBUG("[DONE] Reading CollageFrame[%d]" , collageBufferFrameIndex_ );

    compositedFramesCount_ = 0 ;
    readOutReady_ = false ;
}

CLFrame<uint> *&CLCompositorAccumulate::getFinalFrame()
{
    return finalFrameReadout_ ;
}

uint CLCompositorAccumulate::framesCount() const
{
    return framesCount_ ;
}

uint8_t CLCompositorAccumulate::getCompositedFramesCount() const
{
    return compositedFramesCount_ ;
}

void CLCompositorAccumulate::initializeBuffers_()
{
    LOG_DEBUG("Initializing Buffers ...");

    finalFrameReadout_ = new CLFrame32( frameDimensions_ );

    LOG_DEBUG("[DONE] Initializing Buffers ...");
}

void CLCompositorAccumulate::initializeKernel_()
{
    LOG_DEBUG( "Initializing an OpenCL Kernel ... " );

    compositingKernel_ =
            new CLXRayCompositingKernel( context_ ,
                                         "xray_compositing_accumulate" );

    LOG_DEBUG( "[DONE] Initializing an OpenCL Kernel ... " );
}
