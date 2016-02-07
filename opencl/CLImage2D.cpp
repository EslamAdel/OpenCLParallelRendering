#include "CLImage2D.h"
#include "Logger.h"

template< class T >
CLImage2D< T >::CLImage2D(const Dimensions2D dimensions,
                          const cl_channel_order channelOrder,
                          const cl_channel_type channelType)
    : CLFrame< T >( dimensions )
{
    imageFormat_.image_channel_order = channelOrder;
    imageFormat_.image_channel_data_type = channelType;
}

template< class T >
void CLImage2D< T >::createDeviceData( cl_context context )
{
    LOG_DEBUG( "Creating an OpenCL image: %dx%d ",
               this->dimensions_.x , this->dimensions_.y );

    // Initially, assume that everything is fine
    cl_int error = CL_SUCCESS;
    this->deviceData_ = clCreateImage2D( context,
                                         CL_MEM_READ_WRITE ,
                                         &imageFormat_ ,
                                         this->dimensions_.x ,
                                         this->dimensions_.y ,
                                         0 ,
                                         NULL ,
                                         &error );
    if( error != CL_SUCCESS )
        oclHWDL::Error::checkCLError( error );

    LOG_DEBUG( "[DONE] Creating an OpenCL image " );

}

template< class T >
void CLImage2D< T >::writeDeviceData( cl_command_queue cmdQueue,
                                      const cl_bool blocking )
{
    const size_t origin[3] = { 0 , 0 , 0 };
    const size_t region[3] = { this->dimensions_.x , this->dimensions_.y , 1 };

    // Initially, assume that everything is fine
    cl_int error = CL_SUCCESS;
    error = clEnqueueWriteImage( cmdQueue, this->deviceData_ , blocking ,
                                 origin , region ,
                                 this->dimensions_.x * sizeof(uint),
                                 this->dimensions_.imageSize() * sizeof(uint) ,
                                 ( const void *) this->hostData_ ,
                                 0 , NULL , NULL );

    if( error != CL_SUCCESS )
        oclHWDL::Error::checkCLError( error );
}

template< class T >
void CLImage2D< T >::readDeviceData( cl_command_queue cmdQueue ,
                                     const cl_bool blocking )
{
    const size_t origin[3] = { 0 , 0 , 0 };
    const size_t
            region[3] = { this->dimensions_.x , this->dimensions_.y , 1 };

    // Initially, assume that everything is fine
    cl_int error = CL_SUCCESS;
    error = clEnqueueReadImage( cmdQueue, this->deviceData_ , blocking ,
                                origin , region ,
                                this->dimensions_.x * sizeof(uint),
                                this->dimensions_.imageSize() * sizeof(uint) ,
                                ( void *) this->hostData_ ,
                                0 , NULL , NULL );
    if( error != CL_SUCCESS )
        oclHWDL::Error::checkCLError( error );
}

#include <CLImage2D.ipp>
