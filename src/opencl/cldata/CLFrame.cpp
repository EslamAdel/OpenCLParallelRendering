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
      region_( dimensions ) ,
      offset_( Dimensions2D( 0 , 0 )),
      pixmapSynchronized_( false ) ,
      inDevice_( false ),
      deviceData_( 0 ),
      pixmap_( 0 ),
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
      pixmap_( 0 ),
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

    if( pixmap_ )
        delete pixmap_ ;

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
    CL_ASSERT( error );


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

    QReadLocker lock( &regionLock_ );

    // Initially, assume that everything is fine
    cl_int error = CL_SUCCESS;
    error = clEnqueueWriteBuffer( cmdQueue , deviceData_ , blocking ,
                                  offset_.imageSize( ) * pixelSize( ) ,
                                  region_.imageSize( ) * pixelSize( ) ,
                                  ( const void *) hostData_ ,
                                  0 , 0 , &clTransferEvent_ );
    CL_ASSERT( error );


    evaluateTransferTime_();

}

template< class T >
void CLFrame< T >::readDeviceData( cl_command_queue cmdQueue ,
                                   const cl_bool blocking )
{
    cl_int error = CL_SUCCESS;

    QReadLocker lock( &regionLock_ );

    error = clEnqueueReadBuffer( cmdQueue, deviceData_ , blocking ,
                                 offset_.imageSize( ) * pixelSize( ) ,
                                 region_.imageSize( ) * pixelSize( ) ,
                                 ( void * ) hostData_ ,
                                 0 , 0 , &clTransferEvent_ );

    CL_ASSERT( error );

    //Now, neither QPixmap frame_ nor rgbaFrame represents the recent raw data.
    pixmapSynchronized_ = false ;

    evaluateTransferTime_();

}

template< class T >
void CLFrame< T >::readOtherDeviceData(
        cl_command_queue sourceCmdQueue ,
        const CLFrame< T > &sourceFrame ,
        const cl_bool blocking )
{
    if( sourceFrame.getFrameDimensions() != dimensions_ )
        LOG_ERROR("Dimensions mismatch!");

    QReadLocker lock( &regionLock_ );

    const uint64_t hostOffset =
            ( sourceFrame.getOffset().x +
              sourceFrame.dimensions_.x * sourceFrame.getOffset().y ) *
            sourceFrame.channelsInPixel( );

    cl_int error = clEnqueueReadBuffer(
                sourceCmdQueue , sourceFrame.getDeviceData() , blocking ,
                sourceFrame.getOffset().imageSize( ) * pixelSize( ) ,
                sourceFrame.getRegion().imageSize( ) * pixelSize( ) ,
                ( void * ) &hostData_[ hostOffset ] ,
                0 , 0 , &sourceFrame.clTransferEvent_ );

    CL_ASSERT( error );

    //Now, neither QPixmap frame_ nor rgbaFrame represents the recent raw data.
    pixmapSynchronized_ = false ;

    sourceFrame.evaluateTransferTime_();
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

    QReadLocker lock( &regionLock_ );

    cl_bool error = CL_SUCCESS ;
    error = clEnqueueCopyBuffer(
                cmdQueue ,
                frame.getDeviceData() , deviceData_ ,
                frame.getOffset( ).imageSize( ) * frame.pixelSize( ) ,
                offset_.imageSize( ) * pixelSize( ) ,
                region_.imageSize( ) * pixelSize( ) ,
                0 , 0 , &clTransferEvent_ ) ;

    CL_ASSERT( error );

    if( blocking )
        clFinish( cmdQueue );

    evaluateTransferTime_();
}


template< class T >
T *CLFrame<T>::getHostData() const
{
    return hostData_;
}

template< class T >
QPixmap &CLFrame<T>::getFramePixmap()
{
    if ( !pixmap_ )
        pixmap_ = new QPixmap();

    if( pixmapSynchronized_ ) return *pixmap_ ;


    QReadLocker lock( &regionLock_ );

    if( channelOrder_ == FRAME_CHANNEL_ORDER::ORDER_RGBA )
    {
        if( clChannelType() == CL_UNORM_INT8 )
            pixmapData_ = reinterpret_cast< uchar* >( hostData_ ) ;
        else
            // Conversion needed.
            for( int i = 0; i < region_.imageSize() ; i++ )
            {
                pixmapData_[ 4 * i ] =
                        static_cast< uchar >( hostData_[  4 * i ] );
                pixmapData_[ 4 * i + 1 ] =
                        static_cast< uchar >( hostData_[  4 * i + 1 ]);
                pixmapData_[ 4 * i + 2 ] =
                        static_cast< uchar >( hostData_[  4 * i + 2 ]);
                pixmapData_[ 4 * i + 3 ] =
                        static_cast< uchar >( hostData_[  4 * i + 3 ]);
            }

        // Create a QImage and send it back to the rendering window.
        const QImage image( pixmapData_,
                            region_.x , region_.y ,
                            QImage::Format_ARGB32);


        *pixmap_ = pixmap_->fromImage( image );

    }

    else if( channelOrder_ == FRAME_CHANNEL_ORDER::ORDER_INTENSITY )
    {
        for( int i = 0; i < region_.imageSize() ; i++)
            pixmapData_[ i ] = F2B( hostData_[i] );

        // Create a QImage and send it back to the rendering window.
        const QImage image( pixmapData_,
                            region_.x , region_.y ,
                            QImage::Format_Grayscale8 );
        *pixmap_ = pixmap_->fromImage( image );
    }

    return *pixmap_ ;

}

template< class T >
void CLFrame< T >::copyHostData( const T *data )
{
    QReadLocker lock( &regionLock_ );

    std::copy( &data[ 0 ] ,
            &data[ region_.imageSize() * channelsInPixel() - 1 ] ,
            hostData_ );
    pixmapSynchronized_ = false ;
}

template< class T >
void CLFrame< T >::copyHostData( const CLFrame< T > &sourceFrame )
{
    QReadLocker lock( &regionLock_ );

    const T *hostData_SOURCE = sourceFrame.getHostData();
    const uint64_t frameSize = sourceFrame.getRegion().imageSize() ;

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
void CLFrame< T >::evaluateTransferTime_() const
{
    QWriteLocker lock( &transferTimeLock_ );

    // Assuming that every thing is going in the right direction.
    cl_int clErrorCode = CL_SUCCESS;

    cl_ulong start , end;

    clErrorCode =
            clWaitForEvents( 1 , &clTransferEvent_ );


    clErrorCode |=
            clGetEventProfilingInfo( clTransferEvent_,
                                     CL_PROFILING_COMMAND_END,
                                     sizeof(cl_ulong),
                                     &end,
                                     0 );

    clErrorCode |=
            clGetEventProfilingInfo( clTransferEvent_,
                                     CL_PROFILING_COMMAND_START,
                                     sizeof(cl_ulong),
                                     &start,
                                     0 );


    CL_ASSERT( clErrorCode );


    transferTime_ = static_cast< float >( end -  start ) / 1e6 ;

}

template< class T >
float CLFrame< T >::getTransferTime() const
{
    QReadLocker lock( &transferTimeLock_ );
    return transferTime_;
}

template< class T >
const Dimensions2D &CLFrame< T >::getOffset() const
{
    QReadLocker lock( &regionLock_ );
    return offset_;
}

template< class T >
void CLFrame< T >::setRegion( const Dimensions2D &offset ,
                              const Dimensions2D &region )
{
    QWriteLocker lock( &regionLock_ );

    offset_ = offset ;
    region_ = region ;

    if( offset_.x + region_.x > dimensions_.x ||
            offset_.y + region_.y > dimensions_.y )
        LOG_ERROR("Region exceeds the frame dimension.");
}

template< class T >
const Dimensions2D &CLFrame< T >::getRegion() const
{
    QReadLocker lock( &regionLock_ );
    return region_;
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
    /**
      TODO: Implement the rules based on the specs in the link:
      https://www.khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/cl_image_format.html
      **/


    return false ;
}


template< class T >
FRAME_CHANNEL_ORDER CLFrame< T >::channelOrder( ) const
{
    return channelOrder_;
}

}
}


#include "CLFrame.ipp"
