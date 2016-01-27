#include "CLContext.h"

#include <Headers.hh>
#include <Volume.h>
#include<oclUtils.h>
#include <system/Utilities.h>
#include <Logger.h>

#include <auxillary/glm/glm.hpp>
#include <auxillary/glm/gtc/matrix_transform.hpp>
#include <auxillary/glm/gtc/type_ptr.hpp>



#define DEG_TO_RAD(x) (x * 0.0174532925199f)

#define LOCAL_SIZE_X    16
#define LOCAL_SIZE_Y    16


template< class T >
CLContext< T >::CLContext(const Volume<T>* volume, const uint64_t gpuIndex )
    : volume_( volume )
    , gpuIndex_( gpuIndex )
{
    kernelInitialized_ = false;
    linearFiltering_ = true;



    /// @note The oclHWDL scans the entire system, and returns a list of
    /// platforms and devices. Since we don't really care about the different
    /// platforms in the system and curious only to get the GPUs, we get a
    /// list of all the GPUs connected to the system in a list and select
    /// one of them based on the given GPU ID.
    initializeContext_( );
    if( volume!=nullptr )
    {
        initializeKernel_( );
        kernelInitialized_ = true;
    }
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
    createPixelBuffer(512, 512);

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
QPixmap* CLContext< T >::getFrame( )
{
    return &frame_;
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
    clReleaseMemObject( clPixelBuffer_ );
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


    inverseMatrix_ = clCreateBuffer( context_,
                                     CL_MEM_READ_ONLY,
                                     12 * sizeof( float ), 0,
                                     &clErrorCode );
    oclHWDL::Error::checkCLError(clErrorCode);

    activeRenderingKernel_->setInverseViewMatrix( inverseMatrix_);

    oclHWDL::Error::checkCLError(clErrorCode);
}

template< class T >
void CLContext< T >::paint(const Coordinates3D &rotation ,
                           const Coordinates3D &translation,
                           const float &volumeDensity ,
                           const float &imageBrightness)
{
    // Use the GLM to create the Model View Matrix.
    // Initialize to identity.
    glm::mat4 glmMVMatrix = glm::mat4( 1.0f );

    // Use quatrenions
    glm::tvec3<float> rotationVector = glm::tvec3<float>( rotation.x,
                                                          rotation.y,
                                                          rotation.z );
    glm::tquat< float > quaternion =
            glm::tquat< float >(DEG_TO_RAD(rotationVector));
    float angle = glm::angle(quaternion);
    glm::tvec3< float > axis = glm::axis(quaternion);
    glm::tvec3< float > translationVector = glm::tvec3<float>( translation.x,
                                                               translation.y,
                                                               4.0);

    glm::tvec3< float > relativeCenter =
            glm::tvec3<float>( volume_->getUnitCubeCenter().x-0.5,
                               volume_->getUnitCubeCenter().y-0.5,
                               volume_->getUnitCubeCenter().z-0.5);

    glm::tvec3< float > relativeCenterBack =
            glm::tvec3<float>( 4*(0.5 - volume_->getUnitCubeCenter().x) ,
                               4*(0.5 - volume_->getUnitCubeCenter().y) ,
                               4*(0.5 - volume_->getUnitCubeCenter().z) );

    glm::tvec3< float > mirror = glm::tvec3< float >( -1.f , -1.f , -1.f);
    // Rotate , and then translate to keep the local rotation
    glmMVMatrix = glm::translate(glmMVMatrix , relativeCenterBack);
    glmMVMatrix = glm::rotate(glmMVMatrix, angle, axis);
//    glmMVMatrix = glm::translate(glmMVMatrix, relativeCenterBack);

    glmMVMatrix = glm::translate(glmMVMatrix, translationVector);
    //glmMVMatrix = glm::scale( glmMVMatrix , mirror );


    // A GL-compatible matrix
    float modelViewMatrix[16];
    int index = 0;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            modelViewMatrix[index++] = glmMVMatrix[i][j];
        }
    }

    inverseMatrixArray_[0] = modelViewMatrix[0];
    inverseMatrixArray_[1] = modelViewMatrix[4];
    inverseMatrixArray_[2] = modelViewMatrix[8];
    inverseMatrixArray_[3] = modelViewMatrix[12];
    inverseMatrixArray_[4] = modelViewMatrix[1];
    inverseMatrixArray_[5] = modelViewMatrix[5];
    inverseMatrixArray_[6] = modelViewMatrix[9];
    inverseMatrixArray_[7] = modelViewMatrix[13];
    inverseMatrixArray_[8] = modelViewMatrix[2];
    inverseMatrixArray_[9] = modelViewMatrix[6];
    inverseMatrixArray_[10] = modelViewMatrix[10];
    inverseMatrixArray_[11] = modelViewMatrix[14];

    renderFrame( inverseMatrixArray_ , volumeDensity , imageBrightness );
    //uploadFrame();
    //frameBufferToPixmap();
}

template < class T >
bool CLContext<T>::kernelInitialized() const
{
    return kernelInitialized_;
}

template< class T >
void CLContext< T >::renderFrame( const float* inverseMatrix,
                                  const float &volumeDensity,
                                  const float &imageBrightness)
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
    oclHWDL::Error::checkCLError(clErrorCode);


}

template< class T >
void CLContext<T>::uploadFrame()
{
    // Assume everything is fine in the begnning
    cl_int clErrorCode = CL_SUCCESS;

    clEnqueueReadBuffer( commandQueue_, clPixelBuffer_, CL_TRUE, 0,
                         sizeof( uint ) * gridSize_[0] * gridSize_[1],
                        frameData_, 0, NULL, NULL );
    oclHWDL::Error::checkCLError(clErrorCode);

}

template< class T >
void CLContext<T>::frameBufferToPixmap()
{
    u_int8_t r, g, b, a;
    uint rgba;

    for(int i = 0; i < gridSize_[0] * gridSize_[1]; i++)
    {
        rgba = frameData_[i];
        SystemUtilities::convertColorToRGBA( rgba, r, g, b, a );

        frameDataRGBA_[4*i] = r;
        frameDataRGBA_[4*i + 1] = g;
        frameDataRGBA_[4*i + 2] = b;
        frameDataRGBA_[4*i + 3] = a;
    }

    // Create a QImage and send it back to the rendering window.
    QImage image(frameDataRGBA_,
                 gridSize_[0], gridSize_[1], QImage::Format_ARGB32);
    frame_ = frame_.fromImage(image);
}



template< class T >
void CLContext< T >::createPixelBuffer( const uint frameWidth,
                                        const uint frameHeight )
{
    const size_t bufferSize = frameWidth * frameWidth * sizeof(u_int8_t) * 4;

    cl_int error = CL_SUCCESS;
    clPixelBuffer_ = clCreateBuffer( context_,
                                     CL_MEM_WRITE_ONLY,
                                     bufferSize,
                                     NULL, &error );
    oclHWDL::Error::checkCLError(error);

    gridSize_[0] = SystemUtilities::roundUp(LOCAL_SIZE_X, frameWidth );
    gridSize_[1] = SystemUtilities::roundUp( LOCAL_SIZE_Y, frameHeight );

    frameDataRGBA_ = new uchar[gridSize_[0] * gridSize_[1] * 4];
    frameData_ = new uint[gridSize_[0] * gridSize_[1]];
    for(int i = 0; i < gridSize_[0] * gridSize_[1]; i++)
        frameData_[i] = 0;

    activeRenderingKernel_->setFrameBuffer(clPixelBuffer_);
    activeRenderingKernel_->setFrameWidth(frameWidth);
    activeRenderingKernel_->setFrameHeight(frameHeight);

    oclCheckErrorEX( error, CL_SUCCESS, 0 );
}

template< class T >
void CLContext<T>::loadNewVolume(const Volume<T> *volume)
{
    volume_ = volume;
    if( kernelInitialized_ )
    {
        clVolume_ = new CLVolume<T>( volume_, VOLUME_CL_UNSIGNED_INT8 );
        if( volumeArray_ )
            clReleaseMemObject( volumeArray_ );

        volumeArray_ = clVolume_->createDeviceVolume( context_ );
        activeRenderingKernel_->setVolumeData(volumeArray_);
    }
    else initializeKernel_();

    kernelInitialized_ = true;

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
