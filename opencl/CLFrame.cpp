#include "CLFrame.h"
#include <oclHWDL/ErrorHandler.h>
#include <typeinfo>
#include <Logger.h>
#include <Utilities.h>

template< class T >
CLFrame< T >::CLFrame( const Dimensions2D dimensions ,
                       T *data )
    : dimensions_( dimensions ) ,
      hostData_( data )
{
    if( typeid(T) != typeid(uint) )
        LOG_ERROR("Images other than uint/pixel is not supported!");

    if( data == nullptr )
        hostData_ = new T[ dimensions.imageSize() ];

    rgbaFrame_ = new uchar[ dimensions.imageSize() * 4 ];

}

template< class T >
CLFrame< T >::~CLFrame()
{
    clReleaseMemObject( deviceData_ );
}

//template< class T >
//cl_mem CLFrame< T >::createDeviceImage( cl_context context )
//{
//    LOG_DEBUG( "Creating an OpenCL image " );

//    // Initially, assume that everything is fine
//    cl_int error = CL_SUCCESS;
//    deviceImage_ = clCreateImage2D( context,
//                                    CL_MEM_READ_WRITE ,
//                                    &imageFormat_ ,
//                                    dimensions_.x , dimensions_.y ,
//                                    dimensions_.x *sizeof(uint),
//                                    NULL ,
//                                    &error );
//    if( error != CL_SUCCESS )
//        oclHWDL::Error::checkCLError( error );

//    LOG_DEBUG( "[DONE] Creating an OpenCL image " );

//    return deviceImage_;
//}

//template< class T >
//void CLFrame< T >::writeDeviceD(cl_command_queue cmdQueue)
//{

//    static const size_t origin[3] = { 0 , 0 , 0 };
//    static const size_t region[3] = { dimensions_.x , dimensions_.y , 1 };

//    // Initially, assume that everything is fine
//    cl_int error = CL_SUCCESS;
//    error = clEnqueueWriteImage( cmdQueue, deviceImage_ , CL_TRUE , origin ,
//                                 region , dimensions_.x * sizeof(uint),
//                                 dimensions_.imageSize() * sizeof(uint) ,
//                                 ( const void *) hostImage_ ,
//                                 0 , NULL , NULL );

//    if( error != CL_SUCCESS )
//        oclHWDL::Error::checkCLError( error );
//}

//template< class T >
//void CLFrame< T >::readDeviceImage(cl_command_queue cmdQueue)
//{

//    static const size_t origin[3] = { 0 , 0 , 0 };
//    static const size_t region[3] = { dimensions_.x , dimensions_.y , 1 };

//    // Initially, assume that everything is fine
//    cl_int error = CL_SUCCESS;
//    error = clEnqueueReadImage( cmdQueue, deviceImage_ , CL_TRUE , origin ,
//                                region , dimensions_.x * sizeof(uint),
//                                dimensions_.imageSize() * sizeof(uint) ,
//                                ( void *) hostImage_ ,
//                                0 , NULL , NULL );
//    if( error != CL_SUCCESS )
//        oclHWDL::Error::checkCLError( error );
//}


template< class T >
void CLFrame< T >::createDeviceData( cl_context context )
{
    LOG_DEBUG( "Creating an OpenCL image " );

    // Initially, assume that everything is fine
    cl_int error = CL_SUCCESS;
    deviceData_ = clCreateBuffer( context,
                                  CL_MEM_READ_WRITE ,
                                  dimensions_.imageSize() *sizeof(uint),
                                  NULL ,
                                  &error );
    if( error != CL_SUCCESS )
        oclHWDL::Error::checkCLError( error );

    LOG_DEBUG( "[DONE] Creating an OpenCL image " );
}

template< class T >
void CLFrame< T >::writeDeviceData( cl_command_queue cmdQueue ,
                                    const cl_bool blocking )
{
    // Initially, assume that everything is fine
    static cl_int error = CL_SUCCESS;
    error = clEnqueueWriteBuffer( cmdQueue, deviceData_ , blocking ,
                                  0 , dimensions_.imageSize() * sizeof(T) ,
                                  ( const void *) hostData_ ,
                                  0 , NULL , NULL );

    if( error != CL_SUCCESS )
        oclHWDL::Error::checkCLError( error );
}

template< class T >
void CLFrame< T >::readDeviceData( cl_command_queue cmdQueue ,
                                   const cl_bool blocking )
{
    static cl_int error = CL_SUCCESS;
    error = clEnqueueReadBuffer( cmdQueue, deviceData_ , blocking ,
                                 0 , dimensions_.imageSize() * sizeof(T) ,
                                 ( void * ) hostData_ ,
                                 0 , NULL , NULL);

//    uint64_t checksum = 0;
//    for( auto i = 0 ; i < dimensions_.imageSize() ; i++ )
//        checksum+= hostData_[ i ];

//    LOG_DEBUG("Final Frame Size:%dx%d" , dimensions_.x , dimensions_.y );
//    LOG_DEBUG("Final Frame Size:%d " ,dimensions_.imageSize() * sizeof(T));
//    LOG_DEBUG("Final Frame Checksum:%d" , checksum );
    if( error != CL_SUCCESS )
        oclHWDL::Error::checkCLError( error );
}


template< class T >
T *CLFrame<T>::getHostData() const
{
    return hostData_;
}

template< class T >
QPixmap &CLFrame<T>::getFramePixmap()
{
    u_int8_t r, g, b, a;
    uint rgba;

    for(int i = 0; i < dimensions_.imageSize() ; i++)
    {
        rgba = hostData_[i];
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

template< class T >
void CLFrame< T >::setHostData( T *data)
{
    if( hostData_ != data )
    {
        delete hostData_ ;
        hostData_ = data ;
    }

}


template< class T >
cl_mem CLFrame< T >::getDeviceData() const
{
    return deviceData_ ;
}

#include <CLFrame.ipp>
