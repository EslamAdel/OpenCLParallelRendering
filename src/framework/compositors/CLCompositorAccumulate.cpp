#include "CLCompositorAccumulate.h"

template< class T >
clpar::Compositor::CLCompositorAccumulate< T >::CLCompositorAccumulate(
        const uint64_t gpuIndex,
        const uint frameWidth ,
        const uint frameHeight ,
        const std::string kernelDirectory )
    : CLAbstractCompositor( gpuIndex  ,frameWidth , frameHeight, kernelDirectory )
{
    compositedFramesCount_ = 0 ;
    framesCount_ = 0 ;

    initializeKernel_( );
    initializeBuffers_( );
}

template< class T >
void clpar::Compositor::CLCompositorAccumulate< T >::allocateFrame(
        Renderer::CLAbstractRenderer *renderer )
{

    if( renderers_.contains( renderer ))
        return ;

    renderer->setFrameIndex( renderers_.size( ));

    renderers_ << renderer ;

    clData::CLFrame< T > *frame =
            new clData::CLFrame< T >(
                renderer->getCLFrame().value< clData::CLImage2D< T > *>()->
                getFrameDimensions( ));

    frame->createDeviceData( context_ );

    frames_[ renderer ] = frame ;

    framesCount_++ ;

}

template< class T >
void clpar::Compositor::CLCompositorAccumulate< T >::collectFrame(
        Renderer::CLAbstractRenderer *renderer ,
        const cl_bool block )
{

    clData::CLFrame< T > *sourceFrame =
            renderer->getCLFrame().value< clData::CLImage2D< T > *>( );

    sourceFrame->readDeviceData( renderer->getCommandQueue() ,
                                 block );

    frames_[ renderer ]->
            copyHostData( sourceFrame->getHostData( ));

    frames_[ renderer ]->
            writeDeviceData( commandQueue_ ,
                             block );


    loadedFrames_.enqueue( frames_[ renderer ] );
}

template< class T >
void clpar::Compositor::CLCompositorAccumulate< T >::composite( )
{
    if( ++compositedFramesCount_ == framesCount_ )
        readOutReady_ = true ;

    //if first frame, it is already written to collageFrame, return.
    if( compositedFramesCount_ == 1 )
    {
        TIC( compositingProfile.compositing_TIMER );

        //        LOG_DEBUG("Frame[%d] as Collage Buffer", frameIndex );
        //make first loaded frame buffer as collage frame.
        finalFrame_ = loadedFrames_.dequeue() ;

        activeCompositingKernel_->
                setFinalFrame( finalFrame_->getDeviceData( ));
        return ;
    }

    const cl_mem currentFrameObject = loadedFrames_.dequeue()->getDeviceData();

    // Assume everything is fine in the begnning
    cl_int clErrorCode = CL_SUCCESS;

    activeCompositingKernel_->setFrame( currentFrameObject );

    const size_t localSize[ ] = { 1 } ;
    const size_t globalSize[ ] = { frameDimensions_.imageSize() };



    clErrorCode = clEnqueueNDRangeKernel( commandQueue_ ,
                                          activeCompositingKernel_->getKernelObject() ,
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
void clpar::Compositor::CLCompositorAccumulate< T >::loadFinalFrame()
{
    TOC( compositingProfile.compositing_TIMER );
    //    LOG_DEBUG("Reading CollageFrame[%d]" , collageBufferFrameIndex_ );
    finalFrameReadout_->readOtherDeviceData( commandQueue_ ,
                                             *finalFrame_ ,
                                             CL_TRUE );
    //    LOG_DEBUG("[DONE] Reading CollageFrame[%d]" , collageBufferFrameIndex_ );

    compositedFramesCount_ = 0 ;
    readOutReady_ = false ;
}

template< class T >
const clpar::clData::CLFrameVariant &
clpar::Compositor::CLCompositorAccumulate< T >::getFinalFrame() const
{
    this->finalFrameVariant_ = QVariant::fromValue( finalFrameReadout_  );
    return this->finalFrameVariant_ ;
}

template< class T >
uint clpar::Compositor::CLCompositorAccumulate< T >::framesCount() const
{
    return framesCount_ ;
}

template< class T >
uint8_t clpar::Compositor::CLCompositorAccumulate< T >::getCompositedFramesCount() const
{
    return compositedFramesCount_ ;
}

template< class T >
void clpar::Compositor::CLCompositorAccumulate< T >::initializeBuffers_()
{
    LOG_DEBUG("Initializing Buffers ...");

    finalFrameReadout_ = new clData::CLFrame< T >( frameDimensions_ );

    LOG_DEBUG("[DONE] Initializing Buffers ...");
}

template< class T >
void clpar::Compositor::CLCompositorAccumulate< T >::initializeKernel_()
{
    LOG_DEBUG( "Initializing an OpenCL Kernel ... " );

    //    activeCompositingKernel_ =
    //            new CLXRayCompositingKernel( context_ ,
    //                                         "xray_compositing_accumulate" );

    LOG_DEBUG( "[DONE] Initializing an OpenCL Kernel ... " );
}

#include "CLCompositorAccumulate.ipp"
