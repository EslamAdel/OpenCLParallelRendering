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
    : CLAbstractCompositor( gpuIndex ) ,
      frameDimensions_( frameWidth , frameHeight )
{
    framesCount_ = 0 ;
    framesInCompositor_ = 0 ;
    depthIndex_ = nullptr;

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

void CLCompositor::collectFrame( CLRenderer *renderer ,
                                 const cl_bool block )
{

#ifdef BENCHMARKING
    imagesArray_->readOtherDeviceData( renderer->getCommandQueue() ,
                                       renderer->getFrameIndex() ,
                                       *renderer->getCLFrame() ,
                                       block );
#else
    renderer->getCLFrame()->
            readDeviceData( renderer->getCommandQueue() ,
                            block );

    imagesArray_->setFrameData( renderer->getFrameIndex() ,
                                renderer->getCLFrame()->getHostData( ));

#endif
    imagesArray_->loadFrameDataToDevice( renderer->getFrameIndex() ,
                                         commandQueue_ ,
                                         block );
    framesInCompositor_++;
}

void CLCompositor::composite()
{
    if( framesInCompositor_ != imagesArray_->size( ))
        return ;

    qStableSort( renderers_.begin() , renderers_.end() ,
                 CLRenderer::lessThan );

    QVector< uint > depthIndex ;

    for( CLRenderer *renderer : renderers_ )
        depthIndex << renderer->getFrameIndex();

//    for( CLRenderer *renderer : renderers_ )
//        LOG_DEBUG("Depth<%d>: %f" ,
//                  renderer->getFrameIndex() ,
//                  renderer->getCurrentCenter().z );


    depthIndex_->setHostData( depthIndex );
    depthIndex_->writeDeviceData( commandQueue_ , CL_TRUE );

    const size_t localSize[ ] = { 1 , 1  } ;
    const size_t globalSize[ ] = { frameDimensions_.x ,
                                   frameDimensions_.y };

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

void CLCompositor::loadFinalFrame()
{
    //    LOG_DEBUG("Reading CollageFrame" );
    finalFrameReadout_->readOtherDeviceData( commandQueue_ ,
                                             *finalFrame_ ,
                                             CL_TRUE );
    //    LOG_DEBUG("[DONE] Reading CollageFrame"  );

    readOutReady_ = false ;
}

CLFrame<uint> *&CLCompositor::getFinalFrame()
{
    return ( CLFrame< uint > *&) finalFrameReadout_  ;
}



uint CLCompositor::framesCount() const
{
    return framesCount_ ;
}


void CLCompositor::initializeBuffers_()
{

    LOG_DEBUG("Initializing Buffers ...");

    finalFrame_ = new CLImage2D< uint >( frameDimensions_ );
    finalFrameReadout_ = new CLImage2D< uint >( frameDimensions_ );

    finalFrame_->createDeviceData( context_ );

    imagesArray_ = new CLImage2DArray< uint >( frameDimensions_.x ,
                                               frameDimensions_.y ,
                                               0 );


    compositingKernel_->setCollageFrame( finalFrame_->getDeviceData( ));
    LOG_DEBUG("[DONE] Initializing Buffers ...");

}

void CLCompositor::initializeKernel_()
{
    LOG_DEBUG( "Initializing an OpenCL Kernel ... " );
    compositingKernel_ =
            new CLXRayCompositingKernel( context_ ,
                                         "xray_compositing_patch" );
    LOG_DEBUG( "[DONE] Initializing an OpenCL Kernel ... " );
}

