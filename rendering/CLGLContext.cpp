#include "CLGLContext.h"

#include <Volume.h>
#include<oclUtils.h>
#include <Utilities.h>
#include <QGLWidget>


// Round Up Division function
size_t RoundUp(int group_size, int global_size)
{
    int r = global_size % group_size;
    if(r == 0)
    {
        return global_size;
    } else
    {
        return global_size + group_size - r;
    }
}

#define LOCAL_SIZE_X    16
#define LOCAL_SIZE_Y    16

template< class T >
CLGLContext< T >::CLGLContext( const Volume<T>* volume,
                                 const uint64_t gpuIndex ,
                                 const DISPLAY_MODE displayMode )
    : volume_( volume )
    , gpuIndex_( gpuIndex )
    , displayMode_( displayMode )
{

    // Initialize the volume parameters
    volumeDensity_ = 0.05f;
    imageBrightness_ = 1.0f;
    tfOffset_ = 0.0f;
    tfScale_ = 1.0f;
    linearFiltering_ = true;


    /// @note The oclHWDL scans the entire system, and returns a list of
    /// platforms and devices. Since we don't really care about the different
    /// platforms in the system and curious only to get the GPUs, we get a
    /// list of all the GPUs connected to the system in a list and select
    /// one of them based on the given GPU ID.
    initalizeContext_( );



    /// add different rendering kernels
    ///
    CLXRayRenderingKernel* xrayRenderingKernel =
            new CLXRayRenderingKernel( context_ );
    renderingKernels_.push_back( xrayRenderingKernel );
    activeRenderingKernel_ =  xrayRenderingKernel;

}

template< class T >
void CLGLContext< T >::updateGridSize( const uint64_t width,
                                        const uint64_t height )
{
    gridSize_[0] = width;
    gridSize_[1] = height;
}

template< class T >
void CLGLContext< T >::updateImageBrightness( const float imageBrightnessValue )
{
    imageBrightness_ = imageBrightnessValue;
}

template< class T >
void CLGLContext< T >::updateVolumeDensity( const float volumeDensityValue )
{
    volumeDensity_ = volumeDensityValue;
}

template< class T >
void CLGLContext< T >::updateTransferFunctionScale( const float tfScaleValue )
{
    tfScale_ = tfScaleValue;
}

template< class T >
void CLGLContext< T >::updateTransferFunctionOffset( const float tfOffsetValue )
{
    tfOffset_ = tfOffsetValue;
}

template< class T >
uint64_t CLGLContext< T >::getGPUIndex() const
{
    return gpuIndex_;
}

template< class T >
void CLGLContext< T >::initalizeContext_()
{
    selectGPU_();
    createCommandQueue_();
}

template< class T >
void CLGLContext< T >::selectGPU_()
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

    // make sure that the device has support to the image.

    // Create the OpenCL context
    oclHWDL::Context* clContext =
            new oclHWDL::Context(selectedGPU,
                                 oclHWDL::INTEROPERABILITY_CL_GL_CONTEXT);
    context_ = clContext->getContext();
}

template< class T >
cl_platform_id CLGLContext< T >::getPlatformId( ) const
{
    return platform_;
}

template< class T >
cl_device_id CLGLContext< T >::getDeviceId( ) const
{
    return device_;
}

template< class T >
cl_context CLGLContext< T >::getContext( ) const
{
    return context_;
}

template< class T >
cl_command_queue CLGLContext< T >::getCommandQueue( ) const
{
    return commandQueue_;
}

template< class T >
cl_kernel CLGLContext< T >::getKernel( ) const
{
    return kernel_;
}

template< class T >
void CLGLContext< T >::createPixelBuffer( const size_t bufferSize )
{
    cl_int clErrorCode = CL_SUCCESS;
    clPixelBuffer_ = clCreateBuffer( context_,
                                     CL_MEM_WRITE_ONLY,
                                     bufferSize,
                                     NULL, &clErrorCode );
    oclHWDL::Error::checkCLError(clErrorCode);
}


template< class T >
void CLGLContext< T >::releasePixelBuffer( )
{
    clReleaseMemObject( clPixelBuffer_ );
}





template< class T >
void CLGLContext< T >::createCommandQueue_()
{
    cl_int clErrorCode;
    commandQueue_ = clCreateCommandQueue( context_,
                                          device_,
                                          0, &clErrorCode );

    oclHWDL::Error::checkCLError(clErrorCode);
}

template< class T >
void CLGLContext< T >::handleKernel(std::string string)
{
//    std::string path = ".";
//    std::string file = "xray.cl";

//    kernelContext_ = new oclHWDL::KernelContext( path, file,
//                                                 context_,
//                                                 NULL,
//                                                 NULL );

//    cl_bool ex_deviceImageSupport;

//    // Make sure that the device has support to this extension
//    // CL_DEVICE_IMAGE_SUPPORT
//    clGetDeviceInfo( device_,
//                     CL_DEVICE_IMAGE_SUPPORT,
//                     sizeof(ex_deviceImageSupport), &ex_deviceImageSupport, NULL);



//    // Build the program
//    std::string buildOptions1 = "-cl-fast-relaxed-math";
//    // buildOptions1 += ex_deviceImageSupport ? " -DIMAGE_SUPPORT" : "";
//    buildOptions1 += " -DIMAGE_SUPPORT";
//    kernelContext_->buildProgram(buildOptions1);


    // kernel_ = kernelContext_->getKernelObject(std::string("xray"));
    kernel_ = activeRenderingKernel_->getKernelObject();

    // Load the volume data into the host memory.



    // Assuming that every thing is going in the right direction.
    cl_int clErrorCode = CL_SUCCESS;





    clVolume_ = new CLVolume<T>( volume_, VOLUME_CL_UNSIGNED_INT8 );
    volumeArray_ = clVolume_->createDeviceVolume( context_ );

    std::cout << "going initialization 4" << std::endl;

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

    std::cout << "going initialization 5" << std::endl;

    // Upload the transfer function to the volume.
    // TODO: Fix the hardcoded values.
    transferFunctionArray_ = clCreateImage2D
            ( context_,
              CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
              &tfFormat, 9, 1, sizeof( float ) * 9 * 4,
              transferFunctionTable, &clErrorCode );
    oclHWDL::Error::checkCLError(clErrorCode);

    std::cout << "going initialization 6" << std::endl;

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

    std::cout << "going initialization 7" << std::endl;

    // Set the final image and sampler arguments
    // NOTE: Check the arguments' numbers
//    clErrorCode = clSetKernelArg( kernel_, 6, sizeof( cl_mem ),
//                                  ( void* ) &volumeArray_ );

    activeRenderingKernel_->setVolumeData(volumeArray_);


std::cout << "going initialization 78" << std::endl;



//    clErrorCode |= clSetKernelArg( kernel_, 9, sizeof( cl_mem ),
//                                   ( void* ) &transferFunctionArray_ );
//    clErrorCode |= clSetKernelArg( kernel_, 7, sizeof( cl_sampler ),
//                                   linearFiltering_ ? &linearVolumeSampler_ :
//                                                      &nearestVolumeSampler_ );

    activeRenderingKernel_->setVolumeSampler(linearFiltering_ ? linearVolumeSampler_ :
                                                                nearestVolumeSampler_);
//    clErrorCode |= clSetKernelArg( kernel_, 11, sizeof( cl_sampler ),
//                                   &transferFunctionSampler_ );

    std::cout << "going initialization 8" << std::endl;
    //}

    // Initialize the ex_deviceInvViewMatrix with a NULL matrix.
    inverseMatrix_ = clCreateBuffer( context_,
                                     CL_MEM_READ_ONLY,
                                     12 * sizeof( float ), 0,
                                     &clErrorCode );
    oclHWDL::Error::checkCLError(clErrorCode);


    std::cout << "going initialization 9" << std::endl;
//    clErrorCode |= clSetKernelArg( kernel_, 5, sizeof( cl_mem ),
//                                   ( void* ) &inverseMatrix_ );

    activeRenderingKernel_->setInverseViewMatrix( inverseMatrix_);

    std::cout << "going initialization 10" << std::endl;
    oclHWDL::Error::checkCLError(clErrorCode);
    std::cout << "initialization done....";
}

template< class T >
void CLGLContext< T >::renderFrame( float* inverseMatrix )
{
    // update the device view matrix

    // Assume everything is fine in the begnning
    cl_int clErrorCode = CL_SUCCESS;

    // If the interoperability mechanisms between OpenGL and OpenCL are enabled,
    // transfer the ownership of the buffer from OpenGL to OpenCL to write the
    // resulting image to it.
    // This buffer will be locked later from the OpenCL memory address space and
    // transfered to OpenGL to display the rendered frame.
    //if( ex_glInteroperabilityEnabled )
    //{
    // Acquire PBO for OpenCL writing

    clErrorCode |= clEnqueueAcquireGLObjects( commandQueue_, 1,
                                              &clPixelBuffer_, 0, 0, 0 );

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

    // Setup the arguments of the rendering kernel and keep searching
    // for the errors.
//    clErrorCode |= clSetKernelArg( kernel_, 3, sizeof( float ),
//                                   &volumeDensity_ );
//    clErrorCode |= clSetKernelArg( kernel_, 4, sizeof( float ),
//                                   &imageBrightness_ );


    activeRenderingKernel_->setVolumeDensityFactor( volumeDensity_);
    activeRenderingKernel_->setImageBrightnessFactor(imageBrightness_);
//    clErrorCode |= clSetKernelArg( kernel_, 5, sizeof( float ),
//                                   &tfOffset_ );
//    clErrorCode |= clSetKernelArg( kernel_, 6, sizeof( float ),
//                                   &tfScale_ );


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


    // After rendering the frame to the buffer, transfer the ownership back from
    // OpenCL to OpenGL.
    //if( ex_glInteroperabilityEnabled )
    //{
    clErrorCode |= clEnqueueReleaseGLObjects( commandQueue_,
                                              1,
                                              &clPixelBuffer_,
                                              0,
                                              0,
                                              0);
    oclHWDL::Error::checkCLError(clErrorCode);
    clFinish( commandQueue_ );

    //}

    //    // Otherwise, make an explicit copy by mapping the OpenGL PBO to copy the
    //    // data from the OpenCL buffer via the host.
    //    else
    //    {
    //        // Explicit Copy
    //        glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, glPBO_ );

    //        // Map the buffer object into the client's memory
    //        GLubyte* ptr = ( GLubyte* ) glMapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB,
    //                                                    GL_WRITE_ONLY_ARB );

    //        clEnqueueReadBuffer( ex_clCommandQueue,
    //                             ex_clPBO,
    //                             CL_TRUE,
    //                             0,
    //                             sizeof( unsigned int ) * imageHeight_ * imageWidth_,
    //                             ptr,
    //                             0,
    //                             NULL,
    //                             NULL );
    //        // oclCheckErrorEX( clErrorCode, CL_SUCCESS, pCleanup );
    //        glUnmapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB );
    //    }
}


template< class T >
void CLGLContext< T >::registerGLPixelBuffer( const uint glPixelBuffer,
                                               const uint bufferWidth,
                                               const uint bufferHeight )
{
    cl_int error = CL_SUCCESS;

    clPixelBuffer_ = clCreateFromGLBuffer( context_,
                                           CL_MEM_WRITE_ONLY,
                                           glPixelBuffer,
                                           &error );


    gridSize_[0] = RoundUp( LOCAL_SIZE_X, bufferWidth );
    gridSize_[1] = RoundUp( LOCAL_SIZE_Y, bufferHeight );

//    error |= clSetKernelArg( kernel_, 0, sizeof( cl_mem ),
//                             ( void* ) &clPixelBuffer_);
//    error |= clSetKernelArg( kernel_, 1, sizeof( uint ), &bufferWidth );
//    error |= clSetKernelArg( kernel_, 2, sizeof( uint ), &bufferHeight );

    activeRenderingKernel_->setFrameBuffer(clPixelBuffer_);
    activeRenderingKernel_->setFrameWidth(bufferWidth);
    activeRenderingKernel_->setFrameHeight(bufferHeight);

    oclCheckErrorEX( error, CL_SUCCESS, 0 );

}

template< class T >
void CLGLContext< T >::freeBuffers_( )
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
CLGLContext< T >::~CLGLContext( )
{
    freeBuffers_();
}

#include <CLGLContext.ipp>
