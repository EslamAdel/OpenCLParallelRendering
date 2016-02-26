#include "CLImage2D.h"
#include "Logger.h"

template< class T >
CLImage2D< T >::CLImage2D( const Dimensions2D dimensions ,
                           const cl_channel_order channelOrder ,
                           const cl_channel_type channelType )
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

//    LOG_DEBUG("Creating Device 2D Image");
    // Initially, assume that everything is fine
    cl_int error = CL_SUCCESS;
    this->deviceData_ = clCreateImage2D( context,
                                         CL_MEM_WRITE_ONLY  ,
                                         &imageFormat_ ,
                                         this->dimensions_.x ,
                                         this->dimensions_.y ,
                                         0 ,
                                         this->hostData_ ,
                                         &error );
    if( error != CL_SUCCESS )
        oclHWDL::Error::checkCLError( error );


    this->context_ = context ;
    this->inDevice_ = true ;
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
                                 this->dimensions_.x * sizeof(T),
                                 this->dimensions_.imageSize() * sizeof(T) ,
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
                                this->dimensions_.x * sizeof(T),
                                this->dimensions_.imageSize() * sizeof(T) ,
                                ( void *) this->hostData_ ,
                                0 , NULL , NULL );
    if( error != CL_SUCCESS )
        oclHWDL::Error::checkCLError( error );
}

template< class T >
void CLImage2D< T >::readOtherDeviceData( cl_command_queue sourceCmdQueue ,
                                          const CLFrame<T> &sourceFrame ,
                                          const cl_bool blocking )
{
    if( sourceFrame.getFrameDimensions() != this->dimensions_ )
        LOG_ERROR("Dimensions mismatch!");

//    LOG_DEBUG("Reading device image %dx%d = %d",
//              this->dimensions_.x , this->dimensions_.y ,
//              this->dimensions_.imageSize( ));

    const size_t origin[3] = { 0 , 0 , 0 };
    const size_t region[] = { this->dimensions_.x , this->dimensions_.y , 1 } ;

    static cl_int error = CL_SUCCESS;

    error = clEnqueueReadImage( sourceCmdQueue , sourceFrame.getDeviceData() ,
                                blocking , origin , region ,
                                this->dimensions_.x * sizeof(T) ,
                                this->dimensions_.imageSize() * sizeof(T) ,
                                ( void * ) this->hostData_ ,
                                0 , NULL , NULL);

    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("OpenCL Error!");
    }
}


#include <CLImage2D.ipp>


