#include "CLBuffer.h"
#include "Logger.h"


namespace clparen {
namespace clData {


template< class T >
CLBuffer< T >::CLBuffer( const u_int64_t size )
{
    deviceData_ = NULL ;
    context_ = NULL ;
    inDevice_ = false ;
    size_ = size ;
    hostData_ = new T[ size ];

}

template< class T >
void CLBuffer< T >::createDeviceData( cl_context context )
{

    cl_int error = CL_SUCCESS ;

    context_ = context ;
    deviceData_ =
            clCreateBuffer( context , CL_MEM_READ_WRITE ,
                            size_ * sizeof( T ) ,  hostData_ ,
                            &error) ;

    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }

}

template< class T >
void CLBuffer< T >::writeDeviceData( cl_command_queue cmdQueue ,
                                     const cl_bool blocking )
{

    cl_int error = CL_SUCCESS ;

    error =
            clEnqueueWriteBuffer( cmdQueue , deviceData_ , blocking ,
                                  0 , size_ * sizeof( T ) , hostData_ ,
                                  0 , 0 , 0 );

    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}

template< class T >
void CLBuffer< T >::readDeviceData( cl_command_queue cmdQueue ,
                                    const cl_bool blocking )
{

    cl_int error ;

    error =
            clEnqueueReadBuffer( cmdQueue , deviceData_ , blocking ,
                                 0 , size_ * sizeof( T ) , hostData_ ,
                                 0 , 0 , 0 );

    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}

template< class T >
void CLBuffer< T >::resize( u_int64_t newSize )
{
    if( size_ == newSize )
        return;

    size_ = newSize ;

    if( void* mem = std::realloc( hostData_ , newSize ))
        hostData_ = static_cast< T* >( mem );
    else
        LOG_ERROR("Bad Alloc!");

    if( deviceData_ != 0 )
    {
        releaseDeviceData_();
        createDeviceData( context_ );
    }
}

template< class T >
T *CLBuffer< T >::getHostData() const
{
    return hostData_ ;
}

template< class T >
void CLBuffer< T >::setHostData( const T *data )
{
    std::copy(  data  , data + size_ ,  hostData_ );

    //    for( int i=0 ; i < size_ ; i++ )
    //        std::cout << hostData_[i] << " " << data[i] << std::endl ;

}

template< class T >
void CLBuffer< T >::setHostData( const QVector<T> &vector )
{
    if( vector.size() != size_ )
        LOG_ERROR("Size Mismatch!");

    setHostData( vector.constData( ));
}

template< class T >
cl_mem CLBuffer< T >::getDeviceData() const
{
    return deviceData_ ;
}

template< class T >
const size_t CLBuffer< T >::size() const
{
    return size_ ;
}

template< class T >
const cl_context CLBuffer< T >::getContext() const
{
    return context_ ;
}

template< class T >
bool CLBuffer< T >::inDevice() const
{
    return inDevice_ ;
}

template< class T >
void CLBuffer< T >::releaseDeviceData_( )
{
    if( deviceData_ != NULL )
        clReleaseMemObject( deviceData_ );

    deviceData_ = NULL ;
}

template< class T >
QDataStream &operator>>( QDataStream &stream ,
                         CLBuffer< T > &buffer )
{

    LOG_ERROR("Not Implemented yet!");
    return stream ;
}

template< class T >
QDataStream &operator<<( QDataStream &stream ,
                         CLBuffer< T > &buffer )
{


    LOG_ERROR("Not Implemented yet!");

    return stream ;
}

}
}

#include "CLBuffer.ipp"


