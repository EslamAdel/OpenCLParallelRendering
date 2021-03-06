#include "CLVolume.h"
#include <typeinfo>
#include <type_traits>
#include <Logger.h>
#include <oclHWDL.h>

namespace clparen {
namespace CLData {


template< class T >
CLVolume< T >::CLVolume( Volume< T >* volume,
                         const VOLUME_PRECISION precision )
    : volume_( volume )
    , precision_(( precision == VOLUME_PRECISION::VOLUME_CL_DEFAULT )?
                      defaultVolumePrecision_() : precision )
{
    imageDescriptor_.image_type = CL_MEM_OBJECT_IMAGE3D ;

    imageDescriptor_.image_width = volume->getDimensions().x ;

    imageDescriptor_.image_height = volume->getDimensions().y ;

    imageDescriptor_.image_depth = volume->getDimensions().z ;

    imageDescriptor_.image_row_pitch =
            volume->getDimensions().x * sizeof( T );

    imageDescriptor_.image_slice_pitch =
            volume->getDimensions().x *
            volume->getDimensions().y * sizeof( T );

    imageDescriptor_.num_mip_levels = 0 ;

    imageDescriptor_.num_samples = 0 ;

    imageDescriptor_.buffer = 0 ;


    flags_ = CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR ;

    // Always use the CL_INTENSITY ( I, I, I, I ) to avoid replicating
    // the data and wasting memory.
    imageFormat_.image_channel_order = CL_INTENSITY;
}

template< class T >
cl_mem CLVolume< T >::createDeviceVolume( cl_context context )
{
    LOG_DEBUG( "Creating an OpenCL volume : %dx%dx%d ",
               volume_->getDimensions().x ,
               volume_->getDimensions().y ,
               volume_->getDimensions().z  );

    // Initially, assume that everything is fine
    cl_int error = CL_SUCCESS;

    switch ( precision_ )
    {
    case( VOLUME_CL_UNSIGNED_INT8 ):
    {
        LOG_DEBUG( "CL_UNORM_INT8" );
        imageFormat_.image_channel_data_type = CL_UNORM_INT8;

        if( typeid( T ) == typeid( uint8_t ))
        {
            // No need to copy the data into another array with a
            // different format.
            deviceData_ =
                    clCreateImage( context,
                                   flags_ ,
                                   &imageFormat_ ,
                                   &imageDescriptor_ ,
                                   ( void* ) volume_->getData() ,
                                   &error );
        }
        else
        {
            // Conversion is needed !
            const size_t volumeSize
                    = volume_->getDimensions().volumeSize( );

            uint8_t *volumeData = new uint8_t[ volumeSize ];

            for( uint64_t i = 0 ; i <  volumeSize ;  i++ )
            {
                volumeData[ i ] =
                        static_cast< uint8_t >( volume_->getData()[ i ] );
            }

            deviceData_ =
                    clCreateImage( context,
                                   flags_ ,
                                   &imageFormat_ ,
                                   &imageDescriptor_ ,
                                   ( void* ) volumeData ,
                                   &error );

            // Free the converted data
            delete [] volumeData;
        }

    }   break;

        imageFormat_.image_channel_data_type = CL_UNORM_INT16;

    case( VOLUME_CL_UNSIGNED_INT16 ):
        if( typeid( volume_ ) == typeid( uint16_t ))
        {

        }
        else
        {

        }
        break;

    case( VOLUME_CL_HALF_FLOAT ):

        imageFormat_.image_channel_data_type = CL_HALF_FLOAT;
        if( typeid( volume_ ) == typeid( half ))
        {

        }
        else
        {

        }
        break;

        imageFormat_.image_channel_data_type = CL_FLOAT;

    case( VOLUME_CL_FLOAT ):
        if( typeid( volume_ ) == typeid( float ))
        {

        }
        else
        {

        }

        break;
    default:
        break;
    }

    CL_ASSERT( error );

    LOG_DEBUG( "[DONE] Creating an OpenCL volume " );

    return deviceData_;
}

template< class T >
cl_mem CLVolume< T >::getDeviceData() const
{
    return deviceData_ ;
}

template< class T >
void CLVolume< T >::writeDeviceData( cl_command_queue cmdQueue ,
                                     const cl_bool blocking )
{
    QMutexLocker lock( &hostDataMutex_ );

    const size_t origin[3] = { 0 , 0 , 0 };
    const size_t region[3] = { volume_->getDimensions().x ,
                               volume_->getDimensions().y ,
                               volume_->getDimensions().z };

    // Initially, assume that everything is fine
    cl_int error = CL_SUCCESS;
    error = clEnqueueWriteImage( cmdQueue, deviceData_ , blocking ,
                                 origin , region ,
                                 volume_->getDimensions().x * sizeof( T )  ,
                                 volume_->getDimensions().x *
                                 volume_->getDimensions().y * sizeof( T ) ,
                                 ( const void * ) volume_->getData() ,
                                 0 , 0 , 0 );

    CL_ASSERT( error );

}

template< class T >
void CLVolume< T >::copyHostData( const T *data )
{
    QMutexLocker lock( &hostDataMutex_ );

    volume_->copyData( data );

}

template< class T >
void CLVolume< T >::copyHostData(
        const BrickParameters< T > &brickParameters )
{
    QMutexLocker lock( &hostDataMutex_ );

    volume_->copyData( brickParameters );
}

template< class T >
VOLUME_PRECISION CLVolume< T >::defaultVolumePrecision_()
{

    if( std::is_same< T , uchar >::value )
        return VOLUME_PRECISION::VOLUME_CL_UNSIGNED_INT8 ;

    if( std::is_same< T , uint16_t >::value )
        return VOLUME_PRECISION::VOLUME_CL_UNSIGNED_INT16 ;

    if( std::is_same< T , uint32_t >::value )
        return VOLUME_PRECISION::VOLUME_CL_UNSIGNED_INT32 ;

    if( std::is_same< T , half >::value )
        return VOLUME_PRECISION::VOLUME_CL_HALF_FLOAT ;

    if( std::is_same< T , float >::value )
        return VOLUME_PRECISION::VOLUME_CL_FLOAT;

    LOG_ERROR("%s is not supported as volume precision!",
                typeid( T ).name());
}

}
}

#include <CLVolume.ipp>
