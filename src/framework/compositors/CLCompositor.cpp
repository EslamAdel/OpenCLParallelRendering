#include "CLCompositor.h"
#include "Logger.h"
#include <oclUtils.h>
#include <Volume.h>
#include <Utilities.h>

#include <auxillary/glm/glm.hpp>
#include <auxillary/glm/gtc/matrix_transform.hpp>
#include <auxillary/glm/gtc/type_ptr.hpp>

#include <QtAlgorithms>


namespace clparen {
namespace Compositor {


template< class T >
CLCompositor< T >::CLCompositor( const uint64_t gpuIndex,
                                 const uint frameWidth ,
                                 const uint frameHeight ,
                                 const std::string kernelDirectory )
    : CLAbstractCompositor( gpuIndex  ,frameWidth , frameHeight, kernelDirectory )
{
    framesCount_ = 0 ;
    framesInCompositor_ = 0 ;
    depthIndex_ = nullptr;

    initializeBuffers_();
    initializeKernel_();

}

template< class T >
CLCompositor< T >::~CLCompositor()
{

}

template< class T >
void CLCompositor< T >::allocateFrame(
        Renderer::CLAbstractRenderer *renderer )
{
    if( renderers_.contains( renderer ))
        return ;

    renderer->setFrameIndex( renderers_.size( ));
    renderers_ << renderer ;


    imagesArray_->resize( renderers_.size( ) ,
                          context_ );

    depthIndex_->resize( renderers_.size( ));


    updateKernelsArguments_();

}

template< class T >
void CLCompositor< T >::collectFrame(
        Renderer::CLAbstractRenderer *renderer ,
        const cl_bool block )
{

    CLData::CLImage2D< T > *sourceFrame =
            renderer->getCLFrame().value< CLData::CLImage2D< T > *>( );

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

    QMutexLocker lock( &criticalMutex_ );

    framesInCompositor_++;
}

template< class T >
void CLCompositor< T >::composite( )
{
    QMutexLocker lock( &criticalMutex_ );

    if( framesInCompositor_ != imagesArray_->size( ))
        return ;

    TIC( compositingProfile.compositing_TIMER );

    qStableSort( renderers_.begin() , renderers_.end() ,
                 Renderer::CLAbstractRenderer::lessThan );

    QVector< uint > depthIndex ;

    for( const Renderer::CLAbstractRenderer *renderer : renderers_ )
        depthIndex << renderer->getFrameIndex();



    depthIndex_->setHostData( depthIndex );
    depthIndex_->writeDeviceData( commandQueue_ , CL_TRUE );

    const size_t localSize[ ] = { LOCAL_SIZE_X , LOCAL_SIZE_Y  } ;
    const size_t globalSize[ ] = { frameDimensions_.x ,
                                   frameDimensions_.y };

    QMutexLocker kernelLock( &switchKernelMutex_ );
    cl_int clErrorCode =
            clEnqueueNDRangeKernel( commandQueue_ ,
                                    activeCompositingKernel_->getKernelObject() ,
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
const CLData::CLFrameVariant
&CLCompositor<T>::getFinalFrame() const
{
    this->finalFrameVariant_.
            setValue(( CLData::CLImage2D< T > *) finalFrameReadout_ );
    return this->finalFrameVariant_ ;
}

template< class T >
const CLData::CLImage2D< T > &CLCompositor< T >::getFinalFrame( bool )
{
    return *finalFrameReadout_;
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

    finalFrame_ = new CLData::CLImage2D< T >( frameDimensions_ );

    finalFrameReadout_ = new CLData::CLImage2D< T >( frameDimensions_ );

    finalFrame_->createDeviceData( context_ );

    imagesArray_ = new CLData::CLImage2DArray< T >(
                frameDimensions_.x ,
                frameDimensions_.y ,
                0 );

    depthIndex_ = new CLData::CLBuffer< uint >( 1 );

    depthIndex_->createDeviceData( context_ );

    LOG_DEBUG("[DONE] Initializing Buffers ...");
}

template< class T >
void CLCompositor< T >::initializeKernel_()
{
    LOG_DEBUG( "Initializing OpenCL Kernels ... " );

    activeCompositingKernel_ =
            compositingKernels_[ CLKernel::RenderingMode::RENDERING_MODE_Xray ];

    for( CLKernel::CLCompositingKernel* compositingKernel :
         compositingKernels_.values( ))
    {
        // Assuming that every thing is going in the right direction.
        cl_int clErrorCode = CL_SUCCESS;

        compositingKernel->setFinalFrame( finalFrame_->getDeviceData( ));

        oclHWDL::Error::checkCLError( clErrorCode );
    }

    LOG_DEBUG( "[DONE] Initializing OpenCL Kernels ... " );
}


template< class T >
void CLCompositor< T >::updateKernelsArguments_()
{

    for( CLKernel::CLCompositingKernel* compositingKernel :
         compositingKernels_.values( ))
    {
        // Assuming that every thing is going in the right direction.
        cl_int clErrorCode = CL_SUCCESS;

        compositingKernel->setDepthIndex( depthIndex_->getDeviceData( ));

        oclHWDL::Error::checkCLError( clErrorCode );

        if( imagesArray_->inDevice())
            compositingKernel->setFrame( imagesArray_->getDeviceData( ));
    }

}



}
}

#include "CLCompositor.ipp"
