#include "CLFrame.h"
#include <oclHWDL/ErrorHandler.h>
#include <typeinfo>
#include <Logger.h>
#include <Utilities.h>

template< class T >
CLFrame< T >::CLFrame(uint *&image ,
                      const Dimensions2D &dimensions ,
                      const Coordinates3D &center )
    : hostImage_( image ) ,
      dimensions_( dimensions ),
      center_( center )
{
    if( typeid(T) != typeid(uint) )
        LOG_ERROR("Images other than uint/pixel is not supported!");


    imageFormat_.image_channel_order = CL_INTENSITY ;
    imageFormat_.image_channel_data_type = CL_UNORM_INT8;

    rgbaFrame_ = new uchar[ dimensions.imageSize() * 4 ];

}

template< class T >
CLFrame< T >::~CLFrame()
{
    clReleaseMemObject( deviceImage_ );
}

template< class T >
cl_mem CLFrame< T >::createDeviceImage( cl_context context )
{
    LOG_DEBUG( "Creating an OpenCL image " );

    // Initially, assume that everything is fine
    cl_int error = CL_SUCCESS;
    deviceImage_ = clCreateImage2D( context,
                                    CL_MEM_READ_WRITE ,
                                    &imageFormat_ ,
                                    dimensions_.x , dimensions_.y ,
                                    dimensions_.x *sizeof(uint),
                                    NULL ,
                                    &error );
    if( error != CL_SUCCESS )
        oclHWDL::Error::checkCLError( error );

    LOG_DEBUG( "[DONE] Creating an OpenCL image " );

    return deviceImage_;
}

template< class T >
void CLFrame< T >::writeDeviceImage(cl_command_queue cmdQueue)
{

    static const size_t origin[3] = { 0 , 0 , 0 };
    static const size_t region[3] = { dimensions_.x , dimensions_.y , 1 };

    // Initially, assume that everything is fine
    cl_int error = CL_SUCCESS;
    error = clEnqueueWriteImage( cmdQueue, deviceImage_ , CL_TRUE , origin ,
                                 region , dimensions_.x * sizeof(uint),
                                 dimensions_.imageSize() * sizeof(uint) ,
                                 ( const void *) hostImage_ ,
                                 0 , NULL , NULL );

    if( error != CL_SUCCESS )
        oclHWDL::Error::checkCLError( error );
}

template< class T >
void CLFrame< T >::readDeviceImage(cl_command_queue cmdQueue)
{

    static const size_t origin[3] = { 0 , 0 , 0 };
    static const size_t region[3] = { dimensions_.x , dimensions_.y , 1 };

    // Initially, assume that everything is fine
    cl_int error = CL_SUCCESS;
    error = clEnqueueReadImage( cmdQueue, deviceImage_ , CL_TRUE , origin ,
                                region , dimensions_.x * sizeof(uint),
                                dimensions_.imageSize() * sizeof(uint) ,
                                ( void *) hostImage_ ,
                                0 , NULL , NULL );
    if( error != CL_SUCCESS )
        oclHWDL::Error::checkCLError( error );
}

template< class T >
uint *CLFrame<T>::getHostImage() const
{
    return hostImage_;
}

template< class T >
 QPixmap &CLFrame<T>::getFramePixmap()
{
    u_int8_t r, g, b, a;
    uint rgba;

    for(int i = 0; i < dimensions_.imageSize() ; i++)
    {
        rgba = hostImage_[i];
        SystemUtilities::convertColorToRGBA( rgba, r, g, b, a );

        rgbaFrame_[4*i] = r;
        rgbaFrame_[4*i + 1] = g;
        rgbaFrame_[4*i + 2] = b;
        rgbaFrame_[4*i + 3] = a;
    }

    // Create a QImage and send it back to the rendering window.
    const QImage image(rgbaFrame_,
                       dimensions_.x , dimensions_.y , QImage::Format_ARGB32);
    frame_ = frame_.fromImage(image);


    return frame_ ;
}

template < class T >
void CLFrame< T >::setHostImage( uint *&image)
{
    hostImage_ = image ;
}

template< class T >
cl_mem CLFrame< T >::getDeviceImage() const
{
    return deviceImage_ ;
}

#include <CLFrame.ipp>
