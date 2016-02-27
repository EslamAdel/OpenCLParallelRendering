#include "CLVolume.h"
#include <typeinfo>
#include <Logger.h>
#include <oclHWDL.h>

template< class T >
CLVolume< T >::CLVolume( const Volume< T >* volume,
                         const VOLUME_PRECISION precision )
    : volume_( volume )
    , precision_( precision )
{

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

    // Volume format
    cl_image_format format;

    // Always use the CL_INTENSITY ( I, I, I, I ) to avoid replicating
    // the data and wasting memory.
    format.image_channel_order = CL_INTENSITY;



    const Dimensions3D dimensions = volume_->getDimensions();
    T* originalData = volume_->getData( );


    switch ( precision_ )
    {
    case( VOLUME_CL_UNSIGNED_INT8 ):
    {
        LOG_DEBUG( "CL_UNORM_INT8" );
        format.image_channel_data_type = CL_UNORM_INT8;

        // The array that will be uploaded to the device
        uint8_t* volumeData;

        if( typeid( volume_ ) == typeid( uint8_t ))
        {
            // No need to copy the data into another array with a
            // different format.

            deviceData_ = clCreateImage3D( context,
                             CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                             &format,
                             dimensions.x, dimensions.y, dimensions.z,
                             dimensions.x, dimensions.x * dimensions.y,
                             originalData,
                             &error );
        }
        else
        {
            // Conversion is needed !
            volumeData = new uint8_t[ dimensions.volumeSize( ) ];

            for( uint64_t i = 0; i < dimensions.volumeSize( ); i++ )
            {
                volumeData[ i ] = static_cast< uint8_t >( originalData[ i ] );
            }

            deviceData_ = clCreateImage3D( context,
                             CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                             &format,
                             dimensions.x, dimensions.y, dimensions.z,
                             dimensions.x, dimensions.x * dimensions.y,
                             volumeData,
                             &error );

            // Free the converted data
            delete [] volumeData;
        }

    }   break;

        format.image_channel_data_type = CL_UNORM_INT16;

    case( VOLUME_CL_UNSIGNED_INT16 ):
        if( typeid( volume_ ) == typeid( uint16_t ))
        {

        }
        else
        {

        }
        break;

    case( VOLUME_CL_HALF_FLOAT ):

        format.image_channel_data_type = CL_HALF_FLOAT;
        if( typeid( volume_ ) == typeid( half ))
        {

        }
        else
        {

        }
        break;

         format.image_channel_data_type = CL_FLOAT;

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

    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Error Creating Device Volume!");
    }
    LOG_DEBUG( "[DONE] Creating an OpenCL volume " );

    return deviceData_;
}


template< class T >
cl_mem CLVolume< T >::getDeviceData() const
{
    return deviceData_ ;
}

#include <CLVolume.ipp>
