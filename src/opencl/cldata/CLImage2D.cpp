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
    CL_ASSERT( error );

    this->context_ = context ;
    this->inDevice_ = true ;
    LOG_DEBUG( "[DONE] Creating an OpenCL image " );

}


template< class T >
void CLImage2D< T >::writeDeviceData( cl_command_queue cmdQueue,
                                      const cl_bool blocking )
{
    QReadLocker lock( &this->regionLock_ );

    const size_t origin[3] = { this->offset_.x , this->offset_.y , 0 };
    const size_t region[3] = { this->region_.x , this->region_.y , 1 };

    // Initially, assume that everything is fine
    cl_int error = CL_SUCCESS;
    error = clEnqueueWriteImage(
                cmdQueue, this->deviceData_ , blocking ,
                origin , region ,
                this->region_.x * CLFrame< T >::pixelSize()  ,
                this->region_.imageSize() * CLFrame< T >::pixelSize() ,
                ( const void * ) this->hostData_ ,
                0 , 0 , &this->clTransferEvent_ );

    CL_ASSERT( error );
    CL_ASSERT_WARNING( CLFrame< T >::evaluateTransferTime_( ));
}

template< class T >
void CLImage2D< T >::readDeviceData( cl_command_queue cmdQueue ,
                                     const cl_bool blocking )
{
    QReadLocker lock( &this->regionLock_ );

    const size_t origin[3] = { this->offset_.x , this->offset_.y , 0 };
    const size_t region[3] = { this->region_.x , this->region_.y , 1 };

    //    LOG_DEBUG("ImageDim:%s", this->dimensions_.toString().c_str());

    // Initially, assume that everything is fine
    cl_int error = CL_SUCCESS;
    error = clEnqueueReadImage(
                cmdQueue, this->deviceData_ , blocking ,
                origin , region ,
                this->region_.x * CLFrame< T >::pixelSize()  ,
                this->region_.imageSize() * CLFrame< T >::pixelSize() ,
                ( void *) this->hostData_ ,
                0 , 0 , &this->clTransferEvent_ );

    CL_ASSERT( error );
    CL_ASSERT_WARNING( CLFrame< T >::evaluateTransferTime_( ));
}

template< class T >
void CLImage2D< T >::readOtherDeviceData(
        cl_command_queue sourceCmdQueue ,
        const CLFrame< T > &sourceFrame ,
        const cl_bool blocking )
{
    if( sourceFrame.getFrameDimensions() != this->dimensions_ )
        LOG_ERROR("Dimensions mismatch!");

    QReadLocker lock( &this->regionLock_ );

    const size_t origin[3] = { sourceFrame.offset_.x ,
                               sourceFrame.offset_.y , 0 };

    const size_t region[3] = { sourceFrame.region_.x ,
                               sourceFrame.region_.y , 1 };

    cl_int error = CL_SUCCESS;

    const uint64_t hostOffset =
            ( sourceFrame.getOffset().x +
              sourceFrame.dimensions_.x * sourceFrame.getOffset().y ) *
            sourceFrame.channelsInPixel( );

    error = clEnqueueReadImage(
                sourceCmdQueue , sourceFrame.getDeviceData() ,
                blocking , origin , region ,
                sourceFrame.region_.x * CLFrame< T >::pixelSize() ,
                sourceFrame.region_.imageSize() * CLFrame< T >::pixelSize() ,
                ( void * ) &this->hostData_[ hostOffset ] ,
                0 , 0 , &sourceFrame.clTransferEvent_ );

    CL_ASSERT( error );
    CL_ASSERT_WARNING( sourceFrame.evaluateTransferTime_( ));
    //    LOG_DEBUG("offset:(%d,%d),region(%d,%d),hostOffset(%d)",
    //              sourceFrame.offset_.x ,
    //              sourceFrame.offset_.y ,
    //              sourceFrame.region_.x ,
    //              sourceFrame.region_.y ,
    //              hostOffset );


}





}
}

#include <CLImage2D.ipp>

