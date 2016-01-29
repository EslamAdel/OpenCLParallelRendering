#include "CLImage.h"

#include <typeinfo>
#include <Logger.h>

template< class T >
CLImage< T >::CLImage(  Image< T >* image,
                        const IMAGE_PRECISION precision )
    : hostImage_( image ),
      precision_( precision ),
      width_( image->getSizeX() ),
      height_( image->getSizeY() )
{

}

template< class T >
CLImage< T >::~CLImage()
{
    clReleaseMemObject( deviceImage_ );
}

template< class T >
cl_mem CLImage< T >::createDeviceImage( cl_context context )
{
    LOG_DEBUG( "Creating an OpenCL image " );

    if( precision_ != IMAGE_CL_UNSIGNED_INT8 )
        LOG_ERROR("Not handled precision");
    // Initially, assume that everything is fine
    cl_int error = CL_SUCCESS;

    // Volume format
    cl_image_format format;

    // Always use the CL_INTENSITY ( I, I, I, I ) to avoid replicating
    // the data and wasting memory.
    format.image_channel_order = CL_INTENSITY;

    const Dimensions2D dimensions = hostImage_->getDimensions();

    format.image_channel_data_type = CL_UNORM_INT8;

    // The array that will be uploaded to the device



    deviceImage_ = clCreateImage2D( context,
                                    CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                    &format,
                                    width_, height_ ,
                                    width_ ,
                                    hostImage_->getData() ,
                                    &error );


    LOG_DEBUG( "[DONE] Creating an OpenCL image " );

    return deviceImage_;
}


template< class T >
void CLImage< T >::writeDeviceImage(cl_command_queue cmdQueue)
{
    static const size_t origin[3] = { 0 , 0 , 0 };
    static const size_t region[3] = { width_ , height_ , 1 };

    if( precision_ != IMAGE_CL_UNSIGNED_INT8 )
        LOG_ERROR("Not handled precision");

    // Initially, assume that everything is fine
    cl_int error = CL_SUCCESS;
    error = clEnqueueWriteImage( cmdQueue, deviceImage_ , CL_TRUE , origin ,
                                 region , width_ , width_ * height_ ,
                                 ( const void *) hostImage_->getData(),
                                 0 , NULL , NULL );

}

template< class T >
void CLImage< T >::readDeviceImage(cl_command_queue cmdQueue)
{
    if( precision_ != IMAGE_CL_UNSIGNED_INT8 )
        LOG_ERROR("Not handled precision");

    static const size_t origin[3] = { 0 , 0 , 0 };
    static const size_t region[3] = { width_ , height_ , 1 };

    // Initially, assume that everything is fine
    cl_int error = CL_SUCCESS;
    error = clEnqueueReadImage( cmdQueue, deviceImage_ , CL_TRUE , origin ,
                                region , width_ , width_ * height_ ,
                                ( void *) hostImage_->getData(),
                                0 , NULL , NULL );
}

#include <CLImage.ipp>
