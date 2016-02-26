#include "CLCompositor.h"
#include "Logger.h"
#include <oclUtils.h>
#include <Volume.h>
#include <Utilities.h>

#include <auxillary/glm/glm.hpp>
#include <auxillary/glm/gtc/matrix_transform.hpp>
#include <auxillary/glm/gtc/type_ptr.hpp>

#include <QtAlgorithms>

CLCompositor::CLCompositor(const uint64_t gpuIndex,
                           const uint frameWidth ,
                           const uint frameHeight )
    : gpuIndex_( gpuIndex ) ,
      collageFrameDimensions_( frameWidth , frameHeight )
{
    compositedFramesCount_ = 0 ;
    framesCount_ = 0 ;
    framesInCompositor_ = 0 ;
    depthIndex_ = nullptr;

    initializeContext_();
    initializeKernel_();
    initializeBuffers_();

}


CLCompositor::~CLCompositor()
{

}

void CLCompositor::allocateFrame( CLRenderer *renderer )
{
    if( renderers_.contains( renderer ))
        return ;

    renderer->setFrameIndex( renderers_.size( ));

    renderers_ << renderer ;
    imagesArray_->resize( imagesArray_->size() + 1 ,
                          context_ );

    if( depthIndex_ == nullptr )
    {
        depthIndex_ = new CLBuffer< uint >( 1 );
        depthIndex_->createDeviceData( context_ );
    }
    else
        depthIndex_->resize( renderers_.size( ));


    compositingKernel_->setDepthIndex( depthIndex_->getDeviceData( ));

    if( imagesArray_->inDevice())
        compositingKernel_->setFrame( imagesArray_->getDeviceData( ));

}

uint64_t CLCompositor::getGPUIndex() const
{
    return gpuIndex_;
}

CLImage2D< uint > *&CLCompositor::getCLFrameCollage()
{
    return collageFrameReadout_ ;
}


void CLCompositor::collectFrame( CLRenderer *renderer ,
                                 const cl_bool block )
{



    //if the two buffers are in same context, copy the buffer directly in Device.


    //    if( renderer->getContext() ==
    //        frames_[ renderer ]->getContext( ))
    //    {
    //        //TODO: Create the CLCompositor at the same context of the
    //        //CLRenderer that attached to the same GPU.


    //    }
    //    else
    //    {

#ifdef BENCHMARKING
    //direct copy frame from rendering device to the host pointer
    //of the compositing frame.
    //more effiecent.
    //        frames_[ renderer ]->
    //                readOtherDeviceData( renderer->getCommandQueue() ,
    //                                     *renderer->getCLFrame() ,
    //                                     block );

    imagesArray_->readOtherDeviceData( renderer->getCommandQueue() ,
                                       renderer->getFrameIndex() ,
                                       *renderer->getCLFrame() ,
                                       block );
#else
    renderer->getCLFrame()->
            readDeviceData( renderer->getCommandQueue() ,
                            block );

    //        frames_[ renderer ]->
    //                copyHostData( renderer->getCLFrame()->getHostData( ));

    imagesArray_->setFrameData( renderer->getFrameIndex() ,
                                renderer->getCLFrame()->getHostData( ));

#endif

    //        frames_[ renderer ]->
    //                writeDeviceData( commandQueue_ ,
    //                                 block );

    imagesArray_->loadFrameDataToDevice( renderer->getFrameIndex() ,
                                         commandQueue_ ,
                                         block );
    //    }

    framesInCompositor_++;

}

void CLCompositor::accumulateFrame_DEVICE( CLRenderer *renderer )
{   
    //    //if first frame, it is already written to collageFrame, return.
    //    if( compositedFramesCount_ == 0 )
    //    {
    //        //        LOG_DEBUG("Frame[%d] as Collage Buffer", frameIndex );
    //        //make first loaded frame buffer as collage frame.
    //        collageFrame_ = frames_[ renderer ];

    //        compositingKernel_->
    //                setCollageFrame( collageFrame_->getDeviceData( ));

    //        ++compositedFramesCount_;
    //        return ;
    //    }


    //    const CLFrame32 *currentFrame = frames_[ renderer ];


    //    const cl_mem currentFrameObject = currentFrame->getDeviceData();


    //    // Assume everything is fine in the begnning
    //    cl_int clErrorCode = CL_SUCCESS;

    //    compositingKernel_->setFrame( currentFrameObject );

    //    const size_t localSize[ ] = { 1 } ;
    //    const size_t globalSize[ ] = { collageFrameDimensions_.imageSize() };



    //    clErrorCode = clEnqueueNDRangeKernel( commandQueue_ ,
    //                                          compositingKernel_->getKernelObject() ,
    //                                          1 ,
    //                                          NULL ,
    //                                          globalSize ,
    //                                          localSize ,
    //                                          0 ,
    //                                          NULL ,
    //                                          NULL ) ;


    //    if( clErrorCode != CL_SUCCESS )
    //    {
    //        oclHWDL::Error::checkCLError( clErrorCode );
    //        LOG_ERROR("OpenCL Error!");
    //    }

    //    clFinish( commandQueue_ );
    //    //    LOG_DEBUG("[DONE] Accumulating Frame[%d]", frameIndex );

    //    compositedFramesCount_ ++;
}

void CLCompositor::compositeFrames_DEVICE()
{
    if( framesInCompositor_ != imagesArray_->size( ))
        return ;

    qStableSort( renderers_.begin() , renderers_.end() ,
                 CLRenderer::lessThan );

    QVector< uint > depthIndex ;

    for( CLRenderer *renderer : renderers_ )
        depthIndex << renderer->getFrameIndex();

    for( CLRenderer *renderer : renderers_ )
        LOG_DEBUG("Depth<%d>: %f" ,
                  renderer->getFrameIndex() ,
                  renderer->getCurrentCenter().z );


    depthIndex_->setHostData( depthIndex );
    depthIndex_->writeDeviceData( commandQueue_ , CL_TRUE );

    const size_t localSize[ ] = { 1 , 1  } ;
    const size_t globalSize[ ] = { collageFrameDimensions_.x ,
                                   collageFrameDimensions_.y };

    cl_int clErrorCode =
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

    framesInCompositor_ = 0 ;
    readOutReady_ = true ;


}

void CLCompositor::loadCollageFromDevice()
{

//    LOG_DEBUG("Reading CollageFrame" );
    collageFrameReadout_->readOtherDeviceData( commandQueue_ ,
                                               *collageFrame_ ,
                                               CL_TRUE );
//    LOG_DEBUG("[DONE] Reading CollageFrame"  );

    readOutReady_ = false ;
    compositedFramesCount_ = 0 ;
}

uint CLCompositor::framesCount() const
{
    return framesCount_ ;
}

uint8_t CLCompositor::getCompositedFramesCount() const
{
    return compositedFramesCount_ ;
}

bool CLCompositor::readOutReady() const
{
    return readOutReady_ ;
}

void CLCompositor::selectGPU_()
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

void CLCompositor::initializeContext_()
{
    LOG_DEBUG( "Initializing an OpenCL context ... " );

    selectGPU_();
    createCommandQueue_();

    LOG_DEBUG( "[DONE] Initializing an OpenCL context ... " );
}

void CLCompositor::initializeBuffers_()
{

    LOG_DEBUG("Initializing Buffers ...");

    collageFrame_ = new CLImage2D< uint >( collageFrameDimensions_ );
    collageFrameReadout_ = new CLImage2D< uint >( collageFrameDimensions_ );

    collageFrame_->createDeviceData( context_ );

    imagesArray_ = new CLImage2DArray< uint >( collageFrameDimensions_.x ,
                                               collageFrameDimensions_.y ,
                                               0 );


    compositingKernel_->setCollageFrame( collageFrame_->getDeviceData( ));
    LOG_DEBUG("[DONE] Initializing Buffers ...");

}

void CLCompositor::initializeKernel_()
{
    LOG_DEBUG( "Initializing an OpenCL Kernel ... " );

    //    compositingKernel_ =
    //            new CLXRayCompositingKernel( context_ ,
    //                                         "xray_compositing_accumulate" );

    compositingKernel_ =
            new CLXRayCompositingKernel( context_ ,
                                         "xray_compositing_patch" );
    LOG_DEBUG( "[DONE] Initializing an OpenCL Kernel ... " );
}

void CLCompositor::createCommandQueue_()
{
    cl_int clErrorCode = CL_SUCCESS;
    commandQueue_ = clCreateCommandQueue( context_,
                                          device_,
                                          0, &clErrorCode );

    if( clErrorCode != CL_SUCCESS )
        oclHWDL::Error::checkCLError( clErrorCode );
}
