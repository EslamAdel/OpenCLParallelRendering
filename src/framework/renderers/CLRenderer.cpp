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
        const uint frameWidth, const uint frameHeight,
        const Transformation &transformation ,
        const std::string kernelDirectory)
    : CLAbstractRenderer( gpuIndex , frameWidth , frameHeight , kernelDirectory ) ,
      transformation_( transformation )
{
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
    gridSize_[0] = SystemUtilities::roundUp( LOCAL_SIZE_X, frameWidth_ );
    gridSize_[1] = SystemUtilities::roundUp( LOCAL_SIZE_Y, frameHeight_ );

    Dimensions2D dimensions( gridSize_[0] , gridSize_[1]);
    clFrame_ = new clData::CLImage2D< F >( dimensions , CL_INTENSITY , CL_FLOAT );
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
void CLRenderer< V , F >::loadVolume( const VolumeVariant &volume )
{
    volume_ =  volume.value< Volume< V > *>() ;
    currentCenter_ = volume_->getUnitCubeCenter();
    LOG_DEBUG("Loaded Volume D(%d,%d,%d)" , volume_->getDimensions().x ,
              volume_->getDimensions().y , volume_->getDimensions().z );



    // Assume everything is fine in the begnning
    cl_int clErrorCode = CL_SUCCESS;

    // Create an OpenCL volume
    clVolume_ = new clData::CLVolume< V >( volume_ , clData::VOLUME_CL_UNSIGNED_INT8 );

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
const clparen::clData::CLFrameVariant
&CLRenderer< V , F >::getCLFrame() const
{
    this->frameVariant_.setValue(( clData::CLImage2D< F > *) clFrame_ );
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


    activeRenderingKernel_->
            setVolumeDensityFactor( transformation_.volumeDensity );

    activeRenderingKernel_->
            setImageBrightnessFactor( transformation_.brightness );

    // Enqueue the kernel for execution
    clErrorCode |= clEnqueueNDRangeKernel(
                commandQueue_,
                activeRenderingKernel_->getKernelObject() ,
                2,
                NULL,
                gridSize_,
                localSize,
                0,
                0,
                0 );



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


    linearVolumeSampler_ = clCreateSampler( context_, true,
                                            CL_ADDRESS_CLAMP_TO_EDGE,
                                            CL_FILTER_LINEAR, &clErrorCode );


    nearestVolumeSampler_ = clCreateSampler( context_, true,
                                             CL_ADDRESS_REPEAT,
                                             CL_FILTER_NEAREST,
                                             &clErrorCode );

    inverseMatrix_ = clCreateBuffer( context_,
                                     CL_MEM_READ_ONLY,
                                     12 * sizeof( float ), 0,
                                     &clErrorCode );



    if( clErrorCode != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( clErrorCode );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }


    for( const clKernel::RenderingMode mode : renderingKernels_.keys())
    {
        clKernel::CLRenderingKernel* renderingKernel = renderingKernels_[ mode ];

        renderingKernel->setFrameBuffer( clFrame_->getDeviceData() );
        renderingKernel->setFrameWidth( clFrame_->getFrameDimensions().x );
        renderingKernel->setFrameHeight( clFrame_->getFrameDimensions().y );

        renderingKernel->setInverseViewMatrix( inverseMatrix_ );

        renderingKernel->setVolumeData( clVolume_->getDeviceData( ));

        renderingKernel->setVolumeSampler
                ( linearFiltering_ ? linearVolumeSampler_ : nearestVolumeSampler_);
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

    if( clVolume_->getDeviceData( ))
        clReleaseMemObject( clVolume_->getDeviceData( ));

    if( clFrame_->getDeviceData() )
        clReleaseMemObject( clFrame_->getDeviceData( ));

    if( inverseMatrix_ )
        clReleaseMemObject( inverseMatrix_ );

    if( commandQueue_ )
        clReleaseCommandQueue( commandQueue_ );

    if( context_ )
        clReleaseContext(context_);
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


}
}


#include "CLRenderer.ipp"
