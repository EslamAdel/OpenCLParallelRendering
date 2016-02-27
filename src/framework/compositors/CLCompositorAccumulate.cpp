#include "CLCompositorAccumulate.h"

template< class T >
CLCompositorAccumulate< T >::CLCompositorAccumulate( const uint64_t gpuIndex ,
                                                     const uint frameWidth ,
                                                     const uint frameHeight )
    : CLAbstractCompositor( gpuIndex ) ,
      frameDimensions_( Dimensions2D( frameWidth , frameHeight ))
{
    compositedFramesCount_ = 0 ;
    framesCount_ = 0 ;

    initializeKernel_( );
    initializeBuffers_( );
}

template< class T >
void CLCompositorAccumulate< T >::allocateFrame( CLAbstractRenderer *renderer )
{

    if( renderers_.contains( renderer ))
        return ;

    renderer->setFrameIndex( renderers_.size( ));

    renderers_ << renderer ;

    CLFrame< T > *frame =
            new CLFrame< T >( renderer->getCLFrame().value< CLFrame< T > *>()->
                              getFrameDimensions( ));

    frame->createDeviceData( context_ );

    frames_[ renderer ] = frame ;

    framesCount_++ ;

}

template< class T >
void CLCompositorAccumulate< T >::collectFrame( CLAbstractRenderer *renderer ,
                                                const cl_bool block )
{

    CLFrame< T > *sourceFrame =
            renderer->getCLFrame().value< CLFrame< T > *>( );

#ifdef BENCHMARKING
    //    direct copy frame from rendering device to the host pointer
    //    of the compositing frame.
    //    more effiecent.
    frames_[ renderer ]->
            readOtherDeviceData( renderer->getCommandQueue() ,
                                 *sourceFrame ,
                                 block );

#else

    sourceFrame->readDeviceData( renderer->getCommandQueue() ,
                                 block );

    frames_[ renderer ]->
            copyHostData( sourceFrame->getHostData( ));

#endif
    frames_[ renderer ]->
            writeDeviceData( commandQueue_ ,
                             block );


    loadedFrames_.enqueue( frames_[ renderer ] );
}

template< class T >
void CLCompositorAccumulate< T >::composite( )
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

template< class T >
void CLCompositorAccumulate< T >::loadFinalFrame()
{
    //    LOG_DEBUG("Reading CollageFrame[%d]" , collageBufferFrameIndex_ );
    finalFrameReadout_->readOtherDeviceData( commandQueue_ ,
                                             *finalFrame_ ,
                                             CL_TRUE );
    //    LOG_DEBUG("[DONE] Reading CollageFrame[%d]" , collageBufferFrameIndex_ );

    compositedFramesCount_ = 0 ;
    readOutReady_ = false ;
}

template< class T >
CLFrameVariant &CLCompositorAccumulate< T >::getFinalFrame()
{
    this->finalFrameVariant_= QVariant::fromValue( finalFrameReadout_  );
    return this->finalFrameVariant_ ;
}

template< class T >
uint CLCompositorAccumulate< T >::framesCount() const
{
    return framesCount_ ;
}

template< class T >
uint8_t CLCompositorAccumulate< T >::getCompositedFramesCount() const
{
    return compositedFramesCount_ ;
}

template< class T >
void CLCompositorAccumulate< T >::initializeBuffers_()
{
    LOG_DEBUG("Initializing Buffers ...");

    finalFrameReadout_ = new CLFrame< T >( frameDimensions_ );

    LOG_DEBUG("[DONE] Initializing Buffers ...");
}

template< class T >
void CLCompositorAccumulate< T >::initializeKernel_()
{
    LOG_DEBUG( "Initializing an OpenCL Kernel ... " );

    compositingKernel_ =
            new CLXRayCompositingKernel( context_ ,
                                         "xray_compositing_accumulate" );

    LOG_DEBUG( "[DONE] Initializing an OpenCL Kernel ... " );
}

#include "CLCompositorAccumulate.ipp"