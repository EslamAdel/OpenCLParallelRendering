#include "CLFrame.h"
#include <typeinfo>
#include <Logger.h>

template< class T >
CLFrame< T >::CLFrame( const Dimensions2D dimensions  )
    : dimensions_( dimensions ),
      pixmapSynchronized_( false ) ,
      inDevice_( false )
{
    hostData_ = new T[ dimensions.imageSize() ];

    rgbaFrame_ = new uchar[ dimensions.imageSize() * 4  ];

}

template< class T >
CLFrame< T >::CLFrame( )
    : hostData_( nullptr ) ,
      rgbaFrame_( nullptr )
{
    dimensionsDefined_ = false ;
}

template< class T >
CLFrame< T >::~CLFrame()
{
    releaseDeviceData_();

    delete hostData_ ;

    delete rgbaFrame_ ;
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
                                  dimensions_.imageSize() * sizeof( T ) ,
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
                                  0 , dimensions_.imageSize() * sizeof( T ) ,
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
                                 0 , dimensions_.imageSize() * sizeof(T) ,
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
void CLFrame< T >::readOtherDeviceData( cl_command_queue sourceCmdQueue ,
                                        const CLFrame<T> &sourceFrame ,
                                        const cl_bool blocking )
{
    if( sourceFrame.getFrameDimensions() != dimensions_ )
        LOG_ERROR("Dimensions mismatch!");

    static cl_int error = CL_SUCCESS;
    error = clEnqueueReadBuffer( sourceCmdQueue , sourceFrame.getDeviceData() ,
                                 blocking ,
                                 0 , dimensions_.imageSize() * sizeof( T )  ,
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
void CLFrame< T >::copyDeviceData( cl_command_queue cmdQueue ,
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
                                 dimensions_.imageSize() * sizeof( T ) ,
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


    for( int i = 0; i < dimensions_.imageSize() ; i++)
    {
        u_int8_t r, g, b, a;
        uint rgba = hostData_[i];

        convertColorToRGBA_( rgba, r, g, b, a );

        rgbaFrame_[ 4 * i ] = r;
        rgbaFrame_[ 4 * i + 1 ] = g;
        rgbaFrame_[ 4 * i + 2 ] = b;
        rgbaFrame_[ 4 * i + 3 ] = a;
    }

    // Create a QImage and send it back to the rendering window.
    const QImage image( rgbaFrame_,
                        dimensions_.x , dimensions_.y , QImage::Format_ARGB32);
    frame_ = frame_.fromImage( image );

    return frame_ ;

}

template< class T >
void CLFrame< T >::copyHostData( const T *data )
{
    std::copy( &data[ 0 ] , &data[ dimensions_.imageSize() - 1 ] ,
            hostData_ );
    pixmapSynchronized_ = false ;
}

template< class T >
void CLFrame< T >::copyHostData( const CLFrame< T > &sourceFrame )
{
    const T *hostData_SOURCE = sourceFrame.getHostData();
    const uint64_t frameSize = sourceFrame.getFrameDimensions().imageSize() ;

    std::copy( &hostData_SOURCE[ 0 ] ,
            &hostData_SOURCE[ frameSize - 1 ] ,  hostData_ );

    pixmapSynchronized_ = false ;

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

#include <CLFrame.ipp>
