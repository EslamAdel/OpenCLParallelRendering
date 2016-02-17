#include "CLContext.h"

#include <Headers.hh>
#include <Volume.h>
#include <oclUtils.h>
#include <system/Utilities.h>
#include <Logger.h>

#include <auxillary/glm/glm.hpp>
#include <auxillary/glm/gtc/matrix_transform.hpp>
#include <auxillary/glm/gtc/type_ptr.hpp>
#include "ProfilingExterns.h"


#define DEG_TO_RAD(x) (x * 0.0174532925199f)

#define LOCAL_SIZE_X    16
#define LOCAL_SIZE_Y    16


template< class T >
CLContext< T >::CLContext(const uint64_t gpuIndex ,
                          const uint frameWidth ,
                          const uint frameHeight ,
                          const Volume<T>* volume )
    : volume_( volume ) ,
      gpuIndex_( gpuIndex ) ,
      frameWidth_( frameWidth ),
      frameHeight_( frameHeight )
{
    linearFiltering_ = true;


    kernelInitialized_ = false ;
    if( volume == nullptr ) volumeLoaded_ = false ;

    /// @note The oclHWDL scans the entire system, and returns a list of
    /// platforms and devices. Since we don't really care about the different
    /// platforms in the system and curious only to get the GPUs, we get a
    /// list of all the GPUs connected to the system in a list and select
    /// one of them based on the given GPU ID.
    initializeContext_( );

    if( volumeLoaded_ )initializeKernel_( );

}

template< class T >
uint64_t CLContext< T >::getGPUIndex() const
{
    return gpuIndex_;
}

template< class T >
void CLContext< T >::initializeContext_()
{
    LOG_DEBUG( "Initializing an OpenCL context ... " );

    selectGPU_();
    createCommandQueue_();

    LOG_DEBUG( "[DONE] Initializing an OpenCL context ... " );

}

template< class T >
void CLContext<T>::initializeKernel_()
{
    LOG_DEBUG( "Initializing an OpenCL Kernel ... " );

    /// Add all the rendering kernel here, and set the selected to be the
    /// activeRenderingKernel_
    CLXRayRenderingKernel* xrayRenderingKernel =
            new CLXRayRenderingKernel( context_ );
    renderingKernels_.push_back( xrayRenderingKernel );
    activeRenderingKernel_ =  xrayRenderingKernel;

    handleKernel();

    // Creating the pixel buffer that will contain the final image
    createPixelBuffer( frameWidth_ , frameHeight_ );

    LOG_DEBUG( "[DONE] Initializing an OpenCL Kernel ... " );

}

template< class T >
void CLContext< T >::selectGPU_()
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


template< class T >
cl_platform_id CLContext< T >::getPlatformId( ) const
{
    return platform_;
}

template< class T >
cl_device_id CLContext< T >::getDeviceId( ) const
{
    return device_;
}

template< class T >
cl_context CLContext< T >::getContext( ) const
{
    return context_;
}

template< class T >
cl_command_queue CLContext< T >::getCommandQueue( ) const
{
    return commandQueue_;
}

template< class T >
cl_kernel CLContext< T >::getKernel( ) const
{
    return kernel_;
}

template< class T >
void CLContext< T >::releasePixelBuffer( )
{
    delete clFrame_;
}

template< class T >
CLFrame32 *&CLContext< T >::getCLFrame()
{
    return clFrame_;
}

template< class T >
void CLContext< T >::createCommandQueue_()
{
    cl_int clErrorCode;
    commandQueue_ = clCreateCommandQueue( context_,
                                          device_,
                                          0, &clErrorCode );

    oclHWDL::Error::checkCLError(clErrorCode);
}

template< class T >
void CLContext< T >::handleKernel(std::string string)
{
    // Assuming that every thing is going in the right direction.
    cl_int clErrorCode = CL_SUCCESS;

    kernel_ = activeRenderingKernel_->getKernelObject();

    // Create an OpenCL volume
    clVolume_ = new CLVolume<T>( volume_, VOLUME_CL_UNSIGNED_INT8 );
    volumeArray_ = clVolume_->createDeviceVolume( context_ );

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

    // Transfer function format
    cl_image_format tfFormat;
    tfFormat.image_channel_order = CL_RGBA;
    tfFormat.image_channel_data_type = CL_FLOAT;

    // Upload the transfer function to the volume.
    // TODO: Fix the hardcoded values.
    transferFunctionArray_ = clCreateImage2D
                             ( context_,
                               CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                               &tfFormat, 9, 1, sizeof( float ) * 9 * 4,
                               transferFunctionTable, &clErrorCode );
    oclHWDL::Error::checkCLError(clErrorCode);

    // Create samplers (same as texture in OpenGL) for transfer function
    // and the volume for linear interpolation and nearest interpolation.
    transferFunctionSampler_ = clCreateSampler( context_, true,
                                                CL_ADDRESS_CLAMP_TO_EDGE,
                                                CL_FILTER_LINEAR, &clErrorCode );
    oclHWDL::Error::checkCLError(clErrorCode);

    linearVolumeSampler_ = clCreateSampler( context_, true,
                                            CL_ADDRESS_REPEAT,
                                            CL_FILTER_LINEAR, &clErrorCode );
    oclHWDL::Error::checkCLError(clErrorCode);

    nearestVolumeSampler_ = clCreateSampler( context_, true,
                                             CL_ADDRESS_REPEAT,
                                             CL_FILTER_NEAREST,
                                             &clErrorCode );
    oclHWDL::Error::checkCLError(clErrorCode);

    activeRenderingKernel_->setVolumeData(volumeArray_);

    activeRenderingKernel_->setVolumeSampler
            (linearFiltering_ ? linearVolumeSampler_ : nearestVolumeSampler_);

    activeRenderingKernel_->setTransferFunctionData(transferFunctionArray_);

    activeRenderingKernel_->setTransferFunctionSampler(transferFunctionSampler_);

    //Set the initial Value of the Transfer function offset and scale
    //TODO add Slider to update these values.
    float transferOffset = 0.0f;
    float transferScale = 1.0f;

    activeRenderingKernel_->setTransferFunctionOffset(transferOffset);

    activeRenderingKernel_->setTransferFunctionScale(transferScale);

    inverseMatrix_ = clCreateBuffer( context_,
                                     CL_MEM_READ_ONLY,
                                     12 * sizeof( float ), 0,
                                     &clErrorCode );
    oclHWDL::Error::checkCLError(clErrorCode);

    activeRenderingKernel_->setInverseViewMatrix( inverseMatrix_);

    oclHWDL::Error::checkCLError(clErrorCode);
}

template< class T >
void CLContext< T >::paint( const Coordinates3D &rotation ,
                            const Coordinates3D &translation,
                            const float &volumeDensity ,
                            const float &imageBrightness ,
                            const float &transferFScale ,
                            const float &transferFOffset,
                            Coordinates3D &currentCenter )
{

    TIC( RENDERING_PROFILE( gpuIndex_ ).mvMatrix_TIMER );
    // Use the GLM to create the Model View Matrix.
    // Initialize to identity.
    auto glmMVMatrix = glm::mat4( 1.0f );

    // Use quatrenions
    auto rotationVector = glm::tvec3<float>( rotation.x ,
                                             rotation.y ,
                                             rotation.z );
    glm::tquat< float > quaternion =
            glm::tquat< float >(DEG_TO_RAD(rotationVector));
    float angle = glm::angle(quaternion);
    glm::tvec3< float > axis = glm::axis(quaternion);
    auto translationVector = glm::tvec3<float>( translation.x ,
                                                translation.y ,
                                                4-translation.z );

    //Calculating the translate value for each brick
    glm::tvec3< float > relativeCenterBack =
            glm::tvec3<float>( 2.f*(0.5 - volume_->getUnitCubeCenter().x) ,
                               2.f*(0.5 - volume_->getUnitCubeCenter().y) ,
                               2.f*(0.5 - volume_->getUnitCubeCenter().z) );




    //Scale all  to the unit volume
    glm::tvec3< float > unitSccale =
            glm::tvec3< float >(1/volume_->getUnitCubeScaleFactors().x,
                                1/volume_->getUnitCubeScaleFactors().y,
                                1/volume_->getUnitCubeScaleFactors().z);

    //    //Scale at first
    glmMVMatrix = glm::scale(glmMVMatrix, unitSccale);
    //    //Translate each brick to its position
    glmMVMatrix = glm::translate(glmMVMatrix , relativeCenterBack);


    // Rotate , and then translate to keep the local rotation

    glmMVMatrix = glm::rotate( glmMVMatrix , angle , axis );


    glmMVMatrix = glm::translate( glmMVMatrix , translationVector );




    // A GL-compatible matrix
    float modelViewMatrix[16];
    int index = 0 ;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            modelViewMatrix[ index++ ] = glmMVMatrix[ i ][ j ];
        }
    }
    Coordinates3D center;
    // You can then apply this matrix to your vertices with a standard matrix multiplication. Keep in mind that the matrix is arranged in row-major order. With an input vector xIn, the transformed vector xOut is:
    center.x = modelViewMatrix[0] * volume_->getUnitCubeCenter().x +
               modelViewMatrix[1] * volume_->getUnitCubeCenter().y +
               modelViewMatrix[2] * volume_->getUnitCubeCenter().z +
               modelViewMatrix[12];

    center.y = modelViewMatrix[4] * volume_->getUnitCubeCenter().x +
               modelViewMatrix[5] * volume_->getUnitCubeCenter().y +
               modelViewMatrix[6] * volume_->getUnitCubeCenter().z +
               modelViewMatrix[13];

    center.z = modelViewMatrix[8] * volume_->getUnitCubeCenter().x +
               modelViewMatrix[9] * volume_->getUnitCubeCenter().y +
               modelViewMatrix[10] * volume_->getUnitCubeCenter().z+
               modelViewMatrix[14];


    //Set he current center after transformation
    currentCenter = center;

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
    renderFrame( inverseMatrixArray_ , volumeDensity , imageBrightness,
                 transferFScale ,transferFOffset);

    TOC( RENDERING_PROFILE( gpuIndex_ ).rendering_TIMER );
}

template< class T >
void CLContext< T >::renderFrame( const float* inverseMatrix ,
                                  const float &volumeDensity ,
                                  const float &imageBrightness,
                                  const float &transferFScale ,
                                  const float &transferFOffset
                                  )
{
    // update the device view matrix

    // Assume everything is fine in the begnning
    cl_int clErrorCode = CL_SUCCESS;


    clErrorCode |= clEnqueueWriteBuffer( commandQueue_,
                                         inverseMatrix_,
                                         CL_FALSE,
                                         0,
                                         12 * sizeof( float ),
                                         inverseMatrix,
                                         0,
                                         0,
                                         0 );

    // Execute the OpenCL kernel, and write the results to the PBO that is
    // connected now to the OpenCL context.
    size_t localSize[ ] = { LOCAL_SIZE_X, LOCAL_SIZE_Y };


    activeRenderingKernel_->setVolumeDensityFactor( volumeDensity);
    activeRenderingKernel_->setImageBrightnessFactor(imageBrightness);
    activeRenderingKernel_->setTransferFunctionOffset(transferFOffset);
    activeRenderingKernel_->setTransferFunctionScale(transferFScale);

    // Enqueue the kernel for execution
    clErrorCode |= clEnqueueNDRangeKernel( commandQueue_,
                                           kernel_,
                                           2,
                                           NULL,
                                           gridSize_,
                                           localSize,
                                           0,
                                           0,
                                           0);



    clFinish( commandQueue_ );
    if( clErrorCode != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError(clErrorCode);
        LOG_ERROR("Error in Rendering Execution!");
    }
}



template< class T >
void CLContext< T >::createPixelBuffer( const uint frameWidth,
                                        const uint frameHeight )
{

    gridSize_[0] = SystemUtilities::roundUp(LOCAL_SIZE_X, frameWidth );
    gridSize_[1] = SystemUtilities::roundUp( LOCAL_SIZE_Y, frameHeight );

    Dimensions2D dimensions( gridSize_[0] , gridSize_[1]);
    clFrame_ = new CLFrame< uint >( dimensions );
    clFrame_->createDeviceData( context_ );

    activeRenderingKernel_->setFrameBuffer( clFrame_->getDeviceData() );
    activeRenderingKernel_->setFrameWidth(frameWidth);
    activeRenderingKernel_->setFrameHeight(frameHeight);

}

template< class T >
void CLContext<T>::loadVolume_( const Volume<T> *volume )
{

    //TODO : leakage control

    volume_ = volume;

    initializeKernel_();
    kernelInitialized_ = true ;

}

template< class T >
void CLContext< T >::freeBuffers_( )
{
    if( kernel_ )
        clReleaseKernel( kernel_ );

    if( kernelContext_->getProgram() )
        clReleaseProgram( kernelContext_->getProgram() );

    if( linearVolumeSampler_ )
        clReleaseSampler( linearVolumeSampler_ );

    if( nearestVolumeSampler_ )
        clReleaseSampler( nearestVolumeSampler_ );

    if( transferFunctionSampler_ )
        clReleaseSampler( transferFunctionSampler_ );

    if( volumeArray_ )
        clReleaseMemObject( volumeArray_ );

    if( transferFunctionArray_ )
        clReleaseMemObject( transferFunctionArray_ );

    if( clPixelBuffer_ )
        clReleaseMemObject( clPixelBuffer_ );

    if( inverseMatrix_ )
        clReleaseMemObject( inverseMatrix_ );

    if( commandQueue_ )
        clReleaseCommandQueue( commandQueue_ );

    if( context_ )
        clReleaseContext(context_);
}

template< class T >
CLContext< T >::~CLContext( )
{
    freeBuffers_();
}

#include <CLContext.ipp>
