#include "CLRenderer.h"
#include <Logger.h>
#include <auxillary/glm/glm.hpp>
#include <auxillary/glm/gtc/matrix_transform.hpp>
#include <auxillary/glm/gtc/type_ptr.hpp>
#include "ProfilingExterns.h"
#include <system/Utilities.h>

#define DEG_TO_RAD(x) (x * 0.0174532925199f)

#define LOCAL_SIZE_X    16
#define LOCAL_SIZE_Y    16


namespace clparen {
namespace Renderer {

template< class V , class F >
CLRenderer< V , F >::CLRenderer(
        const uint64_t gpuIndex ,
        const Transformation &transformation ,
        const Dimensions2D frameDimensions ,
        const CLData::FRAME_CHANNEL_ORDER channelOrder ,
        const std::string kernelDirectory)
    : CLAbstractRenderer( gpuIndex , frameDimensions ,
                          channelOrder , kernelDirectory ) ,
      transformation_( transformation )
{
    volume_ = 0 ;
    clVolume_ = 0 ;
    clFrame_ = 0 ;
    inverseMatrix_ = 0 ;
    linearVolumeSampler_ = 0 ;
    nearestVolumeSampler_ = 0 ;
    transferFunctionSampler_ = 0 ;
    transferFunction_ = 0 ;



    LOG_INFO( "Creating Context on Node with GPU <%d>", gpuIndex );
    linearFiltering_ = true;


    // Creating the pixel buffer that will contain the final image
    createPixelBuffer_( );
}

template< class V , class F >
CLRenderer< V , F >::~CLRenderer()
{
    freeBuffers_();
}


template< class V , class F >
void CLRenderer< V , F >::createPixelBuffer_()
{
    gridSize_[0] = SystemUtilities::roundUp( LOCAL_SIZE_X, sortFirstDimensions_.x );
    gridSize_[1] = SystemUtilities::roundUp( LOCAL_SIZE_Y, sortFirstDimensions_.y );

    if( clFrame_ != nullptr )
        delete clFrame_ ;

    clFrame_ = new CLData::CLImage2D< F >( sortFirstDimensions_ ,
                                           frameChannelOrder_ );

    //    LOG_DEBUG("Created Frame[%d]:%s",gpuIndex_,sortFirstDimensions_.toString().c_str());

    clFrame_->createDeviceData( context_ );
}


template< class V , class F >
void CLRenderer< V , F >::applyTransformation( )
{
    this->paint_( );

    emit this->finishedRendering( this );
}

template< class V , class F >
const Coordinates3D &CLRenderer< V , F >::getCurrentCenter() const
{
    return currentCenter_ ;
}


template< class V , class F >
void CLRenderer< V , F >::loadVolume( VolumeVariant &volume )
{
    Volume< V > *newVolume = volume.value< Volume< V > *>() ;

    // If no volume yet loaded, initialize the current center.
    if( !volume_ )
        currentCenter_ = newVolume->getUnitCubeCenter();

    volume_ =  newVolume ;

    LOG_DEBUG("Loaded Volume D(%d,%d,%d)" , volume_->getDimensions().x ,
              volume_->getDimensions().y , volume_->getDimensions().z );


    // Assume everything is fine in the begnning
    cl_int clErrorCode = CL_SUCCESS;

    // If already clVolume exists, update!
    if( clVolume_ )
        delete clVolume_ ;

    // Create an OpenCL volume
    clVolume_ = new CLData::CLVolume< V >( volume_ );
    clVolume_->createDeviceVolume( context_ );

    oclHWDL::Error::checkCLError(clErrorCode);


    initializeKernels_();
}

template< class V , class F >
void CLRenderer< V , F >::setFrameIndex( const uint frameIndex )
{
    frameIndex_ = frameIndex;
}

template< class V , class F >
uint CLRenderer< V , F >::getFrameIndex() const
{
    return frameIndex_;
}

template< class V , class F >
const clparen::CLData::CLFrameVariant
&CLRenderer< V , F >::getCLFrame() const
{
    this->frameVariant_.setValue(( CLData::CLImage2D< F > *) clFrame_ );
    return this->frameVariant_ ;
}


template< class V , class F >
void CLRenderer< V , F >::renderFrame()
{
    QMutexLocker lock( &this->switchKernelMutex_ );

    // update the device view matrix

    // Assume everything is fine in the begnning
    cl_int clErrorCode = CL_SUCCESS;


    clErrorCode |= clEnqueueWriteBuffer( commandQueue_,
                                         inverseMatrix_ ,
                                         CL_FALSE,
                                         0,
                                         12 * sizeof( float ),
                                         inverseMatrixArray_ ,
                                         0,
                                         0,
                                         0 );

    // Execute the OpenCL kernel, and write the results to the PBO that is
    // connected now to the OpenCL context.
    const size_t localSize[ ] = { LOCAL_SIZE_X, LOCAL_SIZE_Y };

    const size_t globalWorkOffset[ ] = { sortFirstOffset_.x , sortFirstOffset_.y };

    activeRenderingKernel_->
            setVolumeDensityFactor( transformation_.volumeDensity );
    activeRenderingKernel_->
            setImageBrightnessFactor( transformation_.brightness );
    activeRenderingKernel_->setVolumeIsoValue( transformation_.isoValue );
    activeRenderingKernel_->setStepSize( transformation_.stepSize );
    activeRenderingKernel_->setMaxSteps( transformation_.maxSteps );

    if( activeRenderingKernel_->isUltrasound( ))
    {
        activeRenderingKernel_->setXScale( transformation_.ultrasoundScale.x );
        activeRenderingKernel_->setYScale( transformation_.ultrasoundScale.y );
        activeRenderingKernel_->setZScale( transformation_.ultrasoundScale.z );
        activeRenderingKernel_->setApexAngle( transformation_.apexAngle );
    }
    // Enqueue the kernel for execution
    clErrorCode |= clEnqueueNDRangeKernel(
                commandQueue_,
                activeRenderingKernel_->getKernelObject() ,
                2,
                globalWorkOffset ,
                gridSize_,
                localSize,
                0,
                0,
                &clGPUExecution_ );



    clFinish( commandQueue_ );
    if( clErrorCode != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( clErrorCode );
        LOG_ERROR("Error in Rendering Execution!");
    }
}

template< class V , class F >
void CLRenderer< V , F >::initializeKernels_()
{
    LOG_DEBUG( "Initializing an OpenCL Kernels ... " );

    // Assuming that every thing is going in the right direction.
    cl_int clErrorCode = CL_SUCCESS;

    if( !linearVolumeSampler_ )
        linearVolumeSampler_ = clCreateSampler( context_, true,
                                                CL_ADDRESS_CLAMP_TO_EDGE,
                                                CL_FILTER_LINEAR,
                                                &clErrorCode );


    if( !nearestVolumeSampler_ )
        nearestVolumeSampler_ = clCreateSampler( context_, true,
                                                 CL_ADDRESS_REPEAT,
                                                 CL_FILTER_NEAREST,
                                                 &clErrorCode );

    if( !inverseMatrix_ )
        inverseMatrix_ = clCreateBuffer( context_,
                                         CL_MEM_READ_ONLY,
                                         12 * sizeof( float ), 0,
                                         &clErrorCode );


    // Create transfer function texture (this is the default HEAT MAP)
    float transferFunctionTable[] =
    {
        0.0, 0.0, 0.0, 0.0,
        1.0, 0.0, 0.0, 1.0,
        1.0, 0.5, 0.0, 1.0,
        1.0, 1.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 1.0,
        0.0, 1.0, 1.0, 1.0,
        0.0, 0.0, 1.0, 1.0,
        1.0, 0.0, 1.0, 1.0,
        0.0, 0.0, 0.0, 0.0,
    };

    if( !transferFunction_)
    {
        transferFunction_ = new CLData::CLTransferFunction(
                    9 ,
                    transferFunctionTable );

        transferFunction_->createDeviceData( context_ );
    }
    // Create samplers (same as texture in OpenGL) for transfer function
    // and the volume for linear interpolation and nearest interpolation.
    if( !transferFunctionSampler_ )
        transferFunctionSampler_ = clCreateSampler( context_, true,
                                                    CL_ADDRESS_CLAMP_TO_EDGE,
                                                    CL_FILTER_LINEAR, &clErrorCode );

    if( clErrorCode != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( clErrorCode );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }


    for( const CLKernel::RenderingMode mode : renderingKernels_.keys())
    {
        CLKernel::CLRenderingKernel* renderingKernel = renderingKernels_[ mode ];

        renderingKernel->setFrameBuffer( clFrame_->getDeviceData() );
        renderingKernel->setFrameWidth( frameDimensions_.x );
        renderingKernel->setFrameHeight( frameDimensions_.y );
        renderingKernel->setSortFirstWidth( sortFirstDimensions_.x );
        renderingKernel->setSortFirstHeight( sortFirstDimensions_.y );
        renderingKernel->setInverseViewMatrix( inverseMatrix_ );
        renderingKernel->setVolumeData( clVolume_->getDeviceData( ));

        renderingKernel->setVolumeSampler
                ( linearFiltering_ ? linearVolumeSampler_ : nearestVolumeSampler_);
        renderingKernel->setTransferFunctionSampler( transferFunctionSampler_ );
        renderingKernel->setTransferFunctionData(
                    transferFunction_->getDeviceData());
    }

    LOG_DEBUG( "[DONE] Initializing an OpenCL Kernels ... " );
}

template< class V , class F >
void CLRenderer< V , F >::freeBuffers_()
{

    if( linearVolumeSampler_ )
        clReleaseSampler( linearVolumeSampler_ );

    if( nearestVolumeSampler_ )
        clReleaseSampler( nearestVolumeSampler_ );

    if( clVolume_ && clVolume_->getDeviceData( ))
        clReleaseMemObject( clVolume_->getDeviceData( ));

    if( clFrame_->getDeviceData() )
        clReleaseMemObject( clFrame_->getDeviceData( ));

    if( inverseMatrix_ )
        clReleaseMemObject( inverseMatrix_ );

    if( commandQueue_ )
        clReleaseCommandQueue( commandQueue_ );

    if( context_ )
        clReleaseContext(context_);

    if( transferFunctionSampler_ )
        clReleaseSampler( transferFunctionSampler_ );

    if( transferFunction_ )
        delete transferFunction_ ;
}

template< class V , class F >
void CLRenderer< V , F >::paint_()
{
    TIC( RENDERING_PROFILE( gpuIndex_ ).mvMatrix_TIMER );
    // Use the GLM to create the Model View Matrix.
    // Initialize to identity.
    auto glmMVMatrix = glm::mat4( 1.0f );

    // Use quatrenions
    auto rotationVector =
            glm::tvec3<float>( transformation_.rotation.x ,
                               transformation_.rotation.y ,
                               transformation_.rotation.z );

    glm::tquat< float > quaternion =
            glm::tquat< float >( DEG_TO_RAD( rotationVector ));
    float angle = glm::angle( quaternion );
    glm::tvec3< float > axis = glm::axis( quaternion );

    auto translationVector =
            glm::tvec3< float >( transformation_.translation.x ,
                                 transformation_.translation.y ,
                                 4 - transformation_.translation.z );

    auto scaleVector =
            glm::tvec3< float >( 1 / transformation_.scale.x ,
                                 1 / transformation_.scale.y ,
                                 1 / transformation_.scale.z );

    //Calculating the translate value for each brick
    glm::tvec3< float > relativeCenterBack =
            glm::tvec3<float>( 2.f * ( 0.5 - volume_->getUnitCubeCenter().x ) ,
                               2.f * ( 0.5 - volume_->getUnitCubeCenter().y ) ,
                               2.f * ( 0.5 - volume_->getUnitCubeCenter().z ));


    //Scale all  to the unit volume

    glm::tvec3< float > unitSccale =
            glm::tvec3< float >( 1 / volume_->getUnitCubeScaleFactors().x ,
                                 1 / volume_->getUnitCubeScaleFactors().y ,
                                 1 / volume_->getUnitCubeScaleFactors().z );

    //    //Scale at first

    glmMVMatrix = glm::scale( glmMVMatrix , unitSccale );


    //Translate each brick to its position

    glmMVMatrix = glm::translate( glmMVMatrix , relativeCenterBack );



    // Scale, Rotate, and then translate.

    glmMVMatrix = glm::scale( glmMVMatrix , scaleVector );

    glmMVMatrix = glm::rotate( glmMVMatrix , angle , axis );

    glmMVMatrix = glm::translate( glmMVMatrix , translationVector );



    // A GL-compatible matrix
    float modelViewMatrix[16];
    int index = 0 ;
    for( int i = 0; i < 4; i++ )
    {
        for( int j = 0; j < 4; j++ )
        {
            modelViewMatrix[ index++ ] = glmMVMatrix[ i ][ j ];
        }
    }

    Coordinates3D center ;
    // You can then apply this matrix to your vertices with a standard matrix
    // multiplication. Keep in mind that the matrix is arranged
    // in row-major order. With an input vector xIn,
    // the transformed vector xOut is:
    center.x = modelViewMatrix[0] * volume_->getUnitCubeCenter().x +
            modelViewMatrix[1] * volume_->getUnitCubeCenter().y +
            modelViewMatrix[2] * volume_->getUnitCubeCenter().z +
            modelViewMatrix[12];

    center.y = modelViewMatrix[4] * volume_->getUnitCubeCenter().x +
            modelViewMatrix[5] * volume_->getUnitCubeCenter().y +
            modelViewMatrix[6] * volume_->getUnitCubeCenter().z +
            modelViewMatrix[13];

    center.z = modelViewMatrix[8]  * volume_->getUnitCubeCenter().x +
            modelViewMatrix[9]  * volume_->getUnitCubeCenter().y +
            modelViewMatrix[10] * volume_->getUnitCubeCenter().z +
            modelViewMatrix[14];


    //Set he current center after transformation
    //    currentCenter_ = center;

    //Arrange the matrix in column-major order as expected from the rendering kernel
    inverseMatrixArray_[  0 ] = modelViewMatrix[  0 ];
    inverseMatrixArray_[  1 ] = modelViewMatrix[  4 ];
    inverseMatrixArray_[  2 ] = modelViewMatrix[  8 ];
    inverseMatrixArray_[  3 ] = modelViewMatrix[ 12 ];
    inverseMatrixArray_[  4 ] = modelViewMatrix[  1 ];
    inverseMatrixArray_[  5 ] = modelViewMatrix[  5 ];
    inverseMatrixArray_[  6 ] = modelViewMatrix[  9 ];
    inverseMatrixArray_[  7 ] = modelViewMatrix[ 13 ];
    inverseMatrixArray_[  8 ] = modelViewMatrix[  2 ];
    inverseMatrixArray_[  9 ] = modelViewMatrix[  6 ];
    inverseMatrixArray_[ 10 ] = modelViewMatrix[ 10 ];
    inverseMatrixArray_[ 11 ] = modelViewMatrix[ 14 ];

    TOC( RENDERING_PROFILE( gpuIndex_ ).mvMatrix_TIMER );

    TIC( RENDERING_PROFILE( gpuIndex_ ).rendering_TIMER );

    renderFrame( );

    TOC( RENDERING_PROFILE( gpuIndex_ ).rendering_TIMER );
}


template< class V , class F >
CLData::CLVolume< V > *CLRenderer< V , F >::getCLVolume()
{
    return clVolume_;
}

template< class V , class F >
void CLRenderer< V , F >::reloadVolume( const cl_bool blocking )
{
    clVolume_->writeDeviceData( this->commandQueue_ , blocking );

}

template< class V , class F >
void CLRenderer< V , F >::copyHostData(
        const BrickParameters< V > &brickParameters )
{
    clVolume_->copyHostData( brickParameters );
}

template< class V , class F >
bool CLRenderer< V , F >::isRenderingModeSupported(
        CLKernel::RenderingMode mode )
{
    if( clFrame_ == 0 )
        return false ;

    return renderingKernels_[ mode ]->getChannelOrderSupport()
            == clFrame_->channelOrder() ;
}

template< class V , class F >
void CLRenderer< V , F >::updateTransferFunction( float *transferFunction ,
                                                  uint length )
{

    QMutexLocker lock( &this->switchKernelMutex_ );


    if( !transferFunction_ )
    {
        transferFunction_ =
                new CLData::CLTransferFunction( length ,
                                                transferFunction ) ;
        transferFunction_->createDeviceData( context_ );

    }
    else if ( transferFunction_->length() != length )
    {
        delete transferFunction_;
        transferFunction_ =
                new CLData::CLTransferFunction( length ,
                                                transferFunction ) ;
        transferFunction_->createDeviceData( context_ );
    }
    else
    {
        transferFunction_->copyHostData( transferFunction );
        transferFunction_->writeDeviceData( commandQueue_ , CL_TRUE );
        return ;
    }

    for( const CLKernel::RenderingMode mode : renderingKernels_.keys())
    {
        CLKernel::CLRenderingKernel* renderingKernel = renderingKernels_[ mode ];
        if( renderingKernel->supportTransferFunction( ))
            renderingKernel->setTransferFunctionData(
                        transferFunction_->getDeviceData( ));
    }

}

template< class V , class F >
CLData::FRAME_CHANNEL_ORDER CLRenderer< V , F >::getFrameChannelOrder() const
{
    return frameChannelOrder_ ;
}


}
}


#include "CLRenderer.ipp"
