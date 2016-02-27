#include "CLCompositor.h"
#include "Logger.h"
#include <oclUtils.h>
#include <Volume.h>
#include <Utilities.h>

#include <auxillary/glm/glm.hpp>
#include <auxillary/glm/gtc/matrix_transform.hpp>
#include <auxillary/glm/gtc/type_ptr.hpp>

#include <QtAlgorithms>

template< class T >
CLCompositor< T >::CLCompositor( const uint64_t gpuIndex,
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

template< class T >
CLCompositor< T >::~CLCompositor()
{

}

template< class T >
void CLCompositor< T >::allocateFrame( CLAbstractRenderer *renderer )
{
    if( renderers_.contains( renderer ))
        return ;

    renderer->setFrameIndex( renderers_.size( ));

    renderers_ << renderer ;
    imagesArray_->resize( renderers_.size( ) ,
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

template< class T >
void CLCompositor< T >::collectFrame( CLAbstractRenderer *renderer ,
                                      const cl_bool block )
{

    CLFrame< T > *sourceFrame =
            renderer->getCLFrame().value< CLFrame< T > *>( );

#ifdef BENCHMARKING
    imagesArray_->readOtherDeviceData( renderer->getCommandQueue() ,
                                       renderer->getFrameIndex() ,
                                       *sourceFrame,
                                       block );
#else

    sourceFrame->readDeviceData( renderer->getCommandQueue() ,
                                 block );

    imagesArray_->setFrameData( renderer->getFrameIndex() ,
                                sourceFrame->getHostData( ));
#endif
    imagesArray_->loadFrameDataToDevice( renderer->getFrameIndex() ,
                                         commandQueue_ ,
                                         block );
    framesInCompositor_++;
}

template< class T >
void CLCompositor< T >::composite( )
{
    if( framesInCompositor_ != imagesArray_->size( ))
        return ;

    TIC( compositingProfile.compositing_TIMER );

    qStableSort( renderers_.begin() , renderers_.end() ,
                 CLAbstractRenderer::lessThan );

    QVector< uint > depthIndex ;

    for( CLAbstractRenderer *renderer : renderers_ )
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

    TOC( compositingProfile.compositing_TIMER );

    framesInCompositor_ = 0 ;
    readOutReady_ = true ;
}

template< class T >
void CLCompositor< T >::loadFinalFrame()
{
    finalFrameReadout_->readOtherDeviceData( commandQueue_ ,
                                             *finalFrame_ ,
                                             CL_TRUE );
    readOutReady_ = false ;
}

template< class T >
CLFrameVariant &CLCompositor<T>::getFinalFrame()
{
    this->finalFrameVariant_.setValue(( CLFrame< T > *) finalFrameReadout_ );
    return this->finalFrameVariant_ ;
}

template< class T >
uint CLCompositor< T >::framesCount() const
{
    return framesCount_ ;
}

template< class T >
void CLCompositor< T >::initializeBuffers_()
{
    LOG_DEBUG("Initializing Buffers ...");

    finalFrame_ = new CLImage2D< T >( frameDimensions_ );
    finalFrameReadout_ = new CLImage2D< T >( frameDimensions_ );

    finalFrame_->createDeviceData( context_ );

    imagesArray_ = new CLImage2DArray< T >( frameDimensions_.x ,
                                            frameDimensions_.y ,
                                            0 );

    compositingKernel_->setCollageFrame( finalFrame_->getDeviceData( ));
    LOG_DEBUG("[DONE] Initializing Buffers ...");
}

template< class T >
void CLCompositor< T >::initializeKernel_()
{
    LOG_DEBUG( "Initializing an OpenCL Kernel ... " );
    compositingKernel_ =
            new CLXRayCompositingKernel( context_ ,
                                         "xray_compositing_patch" );
    LOG_DEBUG( "[DONE] Initializing an OpenCL Kernel ... " );
}

#include "CLCompositor.ipp"
