#include "CLFrame.h"
#include <typeinfo>
#include <Logger.h>


// map [0.f , 1.f] => [0 , 255]
#define F2B( f ) (( f ) >= 1.0 ? 255 : (short)( ( f ) * 256.0 ))

namespace clparen {
namespace CLData {


template< class T >
CLFrame< T >::CLFrame( const Dimensions2D dimensions ,
                       const FRAME_CHANNEL_ORDER channelOrder )
    : dimensions_( dimensions ),
      pixmapSynchronized_( false ) ,
      inDevice_( false ),
      deviceData_( nullptr ),
      channelOrder_(( channelOrder == FRAME_CHANNEL_ORDER::ORDER_DEFAULT)?
                        defaultChannelOrder() : channelOrder )
{

    if( isChannelConflict( channelOrder_ ))
        LOG_ERROR("Channel Order conflicts with channel type.");


    hostData_ = new T[ dimensions.imageSize() * channelsInPixel() ];
    pixmapData_ = new uchar[ dimensions.imageSize() * channelsInPixel()  ];


}

template< class T >
CLFrame< T >::CLFrame( const FRAME_CHANNEL_ORDER channelOrder )
    : hostData_( 0 ) ,
      pixmapData_( 0 ),
      pixmapSynchronized_( false ) ,
      inDevice_( false ),
      deviceData_( 0 ),
      channelOrder_(( channelOrder == FRAME_CHANNEL_ORDER::ORDER_DEFAULT)?
                        defaultChannelOrder() : channelOrder )
{
    if( isChannelConflict( channelOrder_ ))
        LOG_ERROR("Channel Order conflicts with channel type.");

    dimensionsDefined_ = false ;
}

template< class T >
CLFrame< T >::~CLFrame()
{
    releaseDeviceData_();

    if( hostData_ )
        delete [] hostData_ ;

    if( pixmapData_ )
        delete [] pixmapData_ ;
}

template< class T >
void CLFrame< T >::createDeviceData( cl_context context ,
                                     const cl_mem_flags flags )
{
    LOG_DEBUG( "Creating an OpenCL image " );

    // Initially, assume that everything is fine
    cl_int error = CL_SUCCESS;
    deviceData_ = clCreateBuffer( context,
                                  flags ,
                                  dimensions_.imageSize() * pixelSize() ,
                                  0 ,
                                  &error );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("OpenCL Error!");
    }

    context_ = context ;
    inDevice_ = true ;

    LOG_DEBUG( "[DONE] Creating an OpenCL image " );

}

template< class T >
void CLFrame< T >::writeDeviceData( cl_command_queue cmdQueue ,
                                    const cl_bool blocking )
{
    if( !inDevice_ )
        LOG_ERROR("No allocation for the buffer in device!");

    // Initially, assume that everything is fine
    cl_int error = CL_SUCCESS;
    error = clEnqueueWriteBuffer( cmdQueue , deviceData_ , blocking ,
                                  0 , dimensions_.imageSize() * pixelSize() ,
                                  ( const void *) hostData_ ,
                                  0 , 0 , 0 );

    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("OpenCL Error!");
    }
}

template< class T >
void CLFrame< T >::readDeviceData( cl_command_queue cmdQueue ,
                                   const cl_bool blocking )
{
    static cl_int error = CL_SUCCESS;
    error = clEnqueueReadBuffer( cmdQueue, deviceData_ , blocking ,
                                 0 , dimensions_.imageSize() * pixelSize() ,
                                 ( void * ) hostData_ ,
                                 0 , 0 , 0);

    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("OpenCL Error!");
    }
    //Now, neither QPixmap frame_ nor rgbaFrame represents the recent raw data.
    pixmapSynchronized_ = false ;
}

template< class T >
void CLFrame< T >::readOtherDeviceData(
        cl_command_queue sourceCmdQueue ,
        const CLFrame<T> &sourceFrame ,
        const cl_bool blocking )
{
    if( sourceFrame.getFrameDimensions() != dimensions_ )
        LOG_ERROR("Dimensions mismatch!");

    static cl_int error = CL_SUCCESS;
    error = clEnqueueReadBuffer( sourceCmdQueue , sourceFrame.getDeviceData() ,
                                 blocking ,
                                 0 , dimensions_.imageSize() * pixelSize()  ,
                                 ( void * ) hostData_ ,
                                 0 , 0 , 0 );

    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("OpenCL Error!");
    }
    //Now, neither QPixmap frame_ nor rgbaFrame represents the recent raw data.
    pixmapSynchronized_ = false ;
}

template< class T >
void CLFrame< T >::copyDeviceData(
        cl_command_queue cmdQueue ,
        const CLFrame< T > &frame ,
        const cl_bool blocking )
{
    if( ! inSameContext( frame ))
        LOG_ERROR("Cannot Copy Device data from different context.");

    if( frame.getFrameDimensions() != dimensions_ )
        LOG_ERROR("Dimensions mismatch!");

    cl_bool error = CL_SUCCESS ;
    error = clEnqueueCopyBuffer( cmdQueue ,
                                 frame.getDeviceData() , deviceData_ ,
                                 0 , 0 ,
                                 dimensions_.imageSize() * pixelSize() ,
                                 0 , 0 , 0 ) ;

    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Error copying buffers");
    }

    if( blocking )
        clFinish( cmdQueue );

}


template< class T >
T *CLFrame<T>::getHostData() const
{
    return hostData_;
}

template< class T >
QPixmap &CLFrame<T>::getFramePixmap()
{
    if( pixmapSynchronized_ ) return frame_ ;


    if( channelOrder_ == FRAME_CHANNEL_ORDER::ORDER_RGBA )
    {
        for( int i = 0; i < dimensions_.imageSize() ; i++)
        {
            pixmapData_[ 4 * i ] =
                    static_cast< uchar >( hostData_[  4 * i ] );
            pixmapData_[ 4 * i + 1 ] =
                    static_cast< uchar >(hostData_[  4 * i + 1 ]);
            pixmapData_[ 4 * i + 2 ] =
                    static_cast< uchar >(hostData_[  4 * i + 2 ]);
            pixmapData_[ 4 * i + 3 ] =
                    static_cast< uchar >(hostData_[  4 * i + 3 ]);

            //            u_int8_t r, g, b, a;
            //            uint rgba = hostData_[i];

            //            convertColorToRGBA_( rgba, r, g, b, a );

            //            pixmapData_[ 4 * i ] = r;
            //            pixmapData_[ 4 * i + 1 ] = g;
            //            pixmapData_[ 4 * i + 2 ] = b;
            //            pixmapData_[ 4 * i + 3 ] = a;
        }

        // Create a QImage and send it back to the rendering window.
        const QImage image( pixmapData_,
                            dimensions_.x , dimensions_.y ,
                            QImage::Format_ARGB32);
        frame_ = frame_.fromImage( image );

    }

    else if( channelOrder_ == FRAME_CHANNEL_ORDER::ORDER_INTENSITY )
    {
        for( int i = 0; i < dimensions_.imageSize() ; i++)
            pixmapData_[ i ] = F2B( hostData_[i] );

        // Create a QImage and send it back to the rendering window.
        const QImage image( pixmapData_,
                            dimensions_.x , dimensions_.y ,
                            QImage::Format_Grayscale8 );
        frame_ = frame_.fromImage( image );
    }

    return frame_ ;

}

template< class T >
void CLFrame< T >::copyHostData( const T *data )
{
    std::copy( &data[ 0 ] ,
            &data[ dimensions_.imageSize() * channelsInPixel() - 1 ] ,
            hostData_ );
    pixmapSynchronized_ = false ;
}

template< class T >
void CLFrame< T >::copyHostData( const CLFrame< T > &sourceFrame )
{
    const T *hostData_SOURCE = sourceFrame.getHostData();
    const uint64_t frameSize = sourceFrame.getFrameDimensions().imageSize() ;

    std::copy( &hostData_SOURCE[ 0 ] ,
            &hostData_SOURCE[ frameSize * channelsInPixel() - 1 ] ,  hostData_ );

    pixmapSynchronized_ = false ;

}

template< class T >
void CLFrame< T >::copyHostData( const Image< T > &sourceFrame )
{

    if( sourceFrame.getDimensions() != dimensions_ )
        LOG_ERROR("Dimensions mismatch!");

    copyHostData( sourceFrame.getData(  ));
}

template< class T >
void CLFrame< T >::setHostData( T *data , bool deepCopy )
{
    if( data == hostData_ )
        return ;
    else if( deepCopy )
        copyHostData( data );
    else
    {
        if( hostData_ != nullptr ) delete [] hostData_ ;
        hostData_ = data ;
        pixmapSynchronized_ = false ;
    }
}


template< class T >
cl_mem CLFrame< T >::getDeviceData() const
{
    return deviceData_ ;
}

template< class T >
const Dimensions2D &CLFrame< T >::getFrameDimensions() const
{
    return dimensions_;
}

template< class T >
const cl_context CLFrame< T >::getContext() const
{
    return context_ ;
}

template< class T >
bool CLFrame< T >::isInDevice() const
{
    return inDevice_ ;
}

template< class T >
bool CLFrame< T >::inSameContext( const CLFrame< T > &frame ) const
{
    return frame.isInDevice() && ( frame.getContext() == context_ ) ;
}

template< class T >
bool CLFrame< T >::dimensionsDefined( ) const
{
    return dimensionsDefined_;
}

template< class T >
void CLFrame< T >::releaseDeviceData_()
{
    if( deviceData_ )
        clReleaseMemObject( deviceData_ );

}


template< class T >
void CLFrame< T >::convertColorToRGBA_( uint Color ,
                                        uint8_t &r ,
                                        uint8_t &g ,
                                        uint8_t &b ,
                                        uint8_t &a )
{

    b = Color & 0xFF; Color >>= 8;
    g = Color & 0xFF; Color >>= 8;
    r = Color & 0xFF; Color >>= 8;
    a = Color & 0xFF;
}

template< class T >
uint8_t CLFrame< T >::pixelSize() const
{
    return ( channelOrder_ == FRAME_CHANNEL_ORDER::ORDER_RGBA )?
                sizeof( T ) * 4 : sizeof( T );
}

template< class T >
uint8_t CLFrame< T >::channelsInPixel() const
{
    return ( channelOrder_ == FRAME_CHANNEL_ORDER::ORDER_RGBA )?
                  4 : 1 ;
}

template< class T >
FRAME_CHANNEL_TYPE CLFrame< T >::frameChannelType()
{
    if( std::is_same< T , uchar >::value )
        return FRAME_CHANNEL_TYPE::FRAME_CL_UNSIGNED_INT8 ;

    if( std::is_same< T , uint16_t >::value )
        return FRAME_CHANNEL_TYPE::FRAME_CL_UNSIGNED_INT16 ;

    if( std::is_same< T , uint32_t >::value )
        return FRAME_CHANNEL_TYPE::FRAME_CL_UNSIGNED_INT32 ;

    if( std::is_same< T , half >::value )
        return FRAME_CHANNEL_TYPE::FRAME_CL_HALF_FLOAT ;

    if( std::is_same< T , float >::value )
        return FRAME_CHANNEL_TYPE::FRAME_CL_FLOAT;
}



template< class T >
FRAME_CHANNEL_ORDER CLFrame< T >::defaultChannelOrder()
{
    // If integers, return RGBA order.
    // If float/half, return intensity channel order.
    /**
    https://www.khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/cl_image_format.html
 **/

    if( std::is_same< T , uint8_t >::value )
        return FRAME_CHANNEL_ORDER::ORDER_RGBA ;

    if( std::is_same< T , uint16_t >::value )
        return FRAME_CHANNEL_ORDER::ORDER_INTENSITY ;

    if( std::is_same< T , half >::value )
        return FRAME_CHANNEL_ORDER::ORDER_INTENSITY ;

    if( std::is_same< T , float >::value )
        return FRAME_CHANNEL_ORDER::ORDER_INTENSITY;


    LOG_ERROR("%s is not supported as frame precision!",
              typeid( T ).name());

}

template< class T >
cl_channel_order CLFrame< T >::clChannelOrder( FRAME_CHANNEL_ORDER order )
{
    switch( order )
    {
    case FRAME_CHANNEL_ORDER::ORDER_INTENSITY :
        return CL_INTENSITY ;
    case FRAME_CHANNEL_ORDER::ORDER_RGBA:
        return CL_RGBA;
    default:
        LOG_WARNING("Not defined");
        return 0;
    }
}

template< class T >
cl_channel_type CLFrame< T >::clChannelType()
{
    if( std::is_same< T , uint8_t >::value )
        return CL_UNORM_INT8 ;

    if( std::is_same< T , uint16_t >::value )
        return CL_UNORM_INT16 ;

    if( std::is_same< T , half >::value )
        return CL_HALF_FLOAT ;

    if( std::is_same< T , float >::value )
        return CL_FLOAT;

    LOG_ERROR("%s is not supported as frame precision!",
              typeid( T ).name());
}


template< class T >
bool CLFrame< T >::isChannelConflict( FRAME_CHANNEL_ORDER order )
{
    const FRAME_CHANNEL_TYPE type = CLFrame< T >::frameChannelType();

    if( order == FRAME_CHANNEL_ORDER::ORDER_RGBA &&
            type != FRAME_CHANNEL_TYPE::FRAME_CL_UNSIGNED_INT8 )
        return true ;

    else return false ;

}


template< class T >
FRAME_CHANNEL_ORDER CLFrame< T >::frameChannelOrder( ) const
{
    return channelOrder_;
}

}
}


#include "CLFrame.ipp"
