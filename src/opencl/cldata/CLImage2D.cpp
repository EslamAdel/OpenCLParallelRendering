#include "CLImage2D.h"
#include "Logger.h"

namespace clparen {
namespace CLData {


template< class T >
CLImage2D< T >::CLImage2D( const Dimensions2D dimensions ,
                           const FRAME_CHANNEL_ORDER channelOrder )
    : CLFrame< T >( dimensions  , channelOrder )
{

    imageFormat_.image_channel_order =
            CLFrame< T >::clChannelOrder( this->channelOrder_ );
    imageFormat_.image_channel_data_type =
            CLFrame< T >::clChannelType();

    imageDescriptor_.image_type = CL_MEM_OBJECT_IMAGE2D ;
    imageDescriptor_.image_width = dimensions.x ;
    imageDescriptor_.image_height = dimensions.y ;
    imageDescriptor_.image_row_pitch = 0 ;
    imageDescriptor_.image_slice_pitch = 0 ;
    imageDescriptor_.num_mip_levels = 0 ;
    imageDescriptor_.num_samples = 0 ;
    imageDescriptor_.buffer = 0 ;

}

template< class T >
CLImage2D< T >::CLImage2D( )
{

}

template< class T >
void CLImage2D< T >::createDeviceData( cl_context context ,
                                       const cl_mem_flags flags )
{
    LOG_DEBUG( "Creating an OpenCL image: %dx%d ",
               this->dimensions_.x , this->dimensions_.y );


    Q_ASSERT( context != nullptr );

    //    LOG_DEBUG("Creating Device 2D Image");
    // Initially, assume that everything is fine
    cl_int error = CL_SUCCESS;
    this->deviceData_ = clCreateImage( context,
                                       flags ,
                                       &imageFormat_ ,
                                       &imageDescriptor_ ,
                                       0 ,
                                       &error );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("OpenCL Error!");
    }

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
    error = clEnqueueWriteImage(
                cmdQueue, this->deviceData_ , blocking ,
                origin , region ,
                this->dimensions_.x * CLFrame< T >::pixelSize()  ,
                this->dimensions_.imageSize() * CLFrame< T >::pixelSize() ,
                ( const void * ) this->hostData_ ,
                0 , 0 , 0 );

    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("OpenCL Error!");
    }
}

template< class T >
void CLImage2D< T >::readDeviceData( cl_command_queue cmdQueue ,
                                     const cl_bool blocking )
{
    const size_t origin[3] = { 0 , 0 , 0 };
    const size_t
            region[3] = { this->dimensions_.x , this->dimensions_.y , 1 };

    //    LOG_DEBUG("ImageDim:%s", this->dimensions_.toString().c_str());

    // Initially, assume that everything is fine
    cl_int error = CL_SUCCESS;
    error = clEnqueueReadImage(
                cmdQueue, this->deviceData_ , blocking ,
                origin , region ,
                this->dimensions_.x * CLFrame< T >::pixelSize()  ,
                this->dimensions_.imageSize() * CLFrame< T >::pixelSize() ,
                ( void *) this->hostData_ ,
                0 , 0 , 0 );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("OpenCL Error!");
    }
}

template< class T >
void CLImage2D< T >::readOtherDeviceData(
        cl_command_queue sourceCmdQueue ,
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

    error = clEnqueueReadImage(
                sourceCmdQueue , sourceFrame.getDeviceData() ,
                blocking , origin , region ,
                this->dimensions_.x * CLFrame< T >::pixelSize() ,
                this->dimensions_.imageSize() * CLFrame< T >::pixelSize() ,
                ( void * ) this->hostData_ ,
                0 , 0 , 0 );

    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("OpenCL Error!");
    }
}





}
}

#include <CLImage2D.ipp>

