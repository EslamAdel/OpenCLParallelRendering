#include "CLImage2DArray.h"
#include "Logger.h"
#include <typeinfo>

namespace clparen {
namespace CLData {


template< class T >
CLImage2DArray< T >::CLImage2DArray(
        const uint width , const uint height ,
        const uint arraySize ,
        const FRAME_CHANNEL_ORDER channelOrder )
    : width_( width ) ,
      height_( height ) ,
      arraySize_( arraySize ) ,
      context_( 0 ) ,
      deviceData_( 0 ),
      channelOrder_(( channelOrder == FRAME_CHANNEL_ORDER::ORDER_DEFAULT)?
                        CLFrame< T >::defaultChannelOrder() : channelOrder )
{
    if( CLFrame< T >::isChannelConflict( channelOrder_ ))
        LOG_ERROR("Channel Order conflicts with channel type.");

    for( auto i = 0 ; i < arraySize ; i++ )
        framesData_.push_back( new T[ width * height * channelsInPixel() ] );



    imageFormat_.image_channel_order =
            CLImage2D< T >::clChannelOrder( channelOrder_ );

    imageFormat_.image_channel_data_type =
            CLImage2D< T >::clChannelType();

    imageDescriptor_.image_type = CL_MEM_OBJECT_IMAGE3D ;
    imageDescriptor_.image_width = width_ ;
    imageDescriptor_.image_height = height_ ;
    imageDescriptor_.image_depth = arraySize_ ;
    imageDescriptor_.image_row_pitch = 0 ;
    imageDescriptor_.image_slice_pitch = 0 ;
    imageDescriptor_.num_mip_levels = 0 ;
    imageDescriptor_.num_samples = 0 ;
    imageDescriptor_.buffer = 0 ;

    inDevice_ = false ;
}

template< class T >
CLImage2DArray< T >::~CLImage2DArray()
{
    if( inDevice_ )
        releaseDeviceData_();

    for( T* data : framesData_ )
    {
        delete [] data ;
    }
    framesData_.clear();

}


template< class T >
void CLImage2DArray< T >::createDeviceData( cl_context context )
{

    if( arraySize_ <= 1 )
    {
        return ;
    }

    if( context == NULL )
        LOG_ERROR("No context specified");

    cl_int error = CL_SUCCESS;

    deviceData_ =
            clCreateImage( context , CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY ,
                           &imageFormat_ , &imageDescriptor_ , 0 , &error ) ;


    if( error != CL_SUCCESS )
        LOG_DEBUG("Failed creating image3d: %dx%dx%d",
                  width_ , height_ , arraySize_ );

    CL_ASSERT( error );


    inDevice_ = true ;
}

template< class T >
void CLImage2DArray< T >::setFrameData(
        const uint index , const T *data )
{
    if( index >= arraySize_ )
        LOG_ERROR("Out of range index!");

    if( data != framesData_[ index ])
    {
        std::copy( &data[0] ,
                &data[ width_ * height_ * channelsInPixel() - 1 ] ,
                &framesData_[ index ][0] );
    }
}


template< class T >
void CLImage2DArray< T >::loadFrameDataToDevice( const uint index ,
                                                 cl_command_queue  commandQueue ,
                                                 cl_bool blocking )
{
    //    if( ! framesSet_[ index ] )
    //        LOG_ERROR("Frame data has not been set!");

    if( index >= size( ))
        LOG_ERROR("index exceeds the limit");


    cl_int error = CL_SUCCESS ;

    const size_t origin[] = { 0 , 0 , index } ;
    const size_t region[] = { width_ , height_ , 1 } ;

    error = clEnqueueWriteImage( commandQueue , deviceData_ , blocking ,
                                 origin , region , width_ * pixelSize()  ,
                                 width_ * height_ * pixelSize()  ,
                                 ( const void * ) framesData_[ index ] , 0 ,
                                 0 , 0 ) ;

    CL_ASSERT( error );
}

template< class T >
cl_mem CLImage2DArray< T >::getDeviceData() const
{
    return deviceData_ ;
}

template< class T>
float CLImage2DArray< T >::getFrameDepth()
{
    return frameDepth_;
}


template< class T >
void CLImage2DArray< T >::resize( const uint newArraySize ,
                                  cl_context context )
{
    if( arraySize_ == newArraySize )
        return ;

    releaseDeviceData_( );
    arraySize_ = newArraySize ;
    imageDescriptor_.image_depth = newArraySize ;

    int delta = (int) newArraySize - (int) framesData_.size( );

    if( delta <= 0 )
        framesData_.resize( newArraySize );

    else
        for( uint i = 0 ; i < delta ; i++ )
            framesData_.push_back( new T[ width_ * height_ * channelsInPixel() ]);


    releaseDeviceData_( );
    createDeviceData(( context_ != NULL )? context_ : context );

}

template< class T >
size_t CLImage2DArray< T >::size( ) const
{
    return arraySize_ ;
}

template< class T >
void CLImage2DArray< T >::readOtherDeviceData( cl_command_queue cmdQueue ,
                                               const uint index,
                                               const CLImage2D< T > &source ,
                                               cl_bool blocking )
{
    if( source.getFrameDimensions() != Dimensions2D( width_ , height_ ))
        LOG_ERROR("Dimensions mismatch!");

    cl_int error = CL_SUCCESS;

    const size_t origin[] = { 0 , 0 , 0 } ;
    const size_t region[] = { width_ , height_ , 1 } ;

    error = clEnqueueReadImage( cmdQueue , source.getDeviceData() , blocking ,
                                origin , region , width_ * pixelSize()  ,
                                width_ * height_ * pixelSize()  ,
                                ( void * ) framesData_[ index ] , 0 ,
                                0 , 0  );

    CL_ASSERT( error );

}

template< class T >
bool CLImage2DArray< T >::inDevice() const
{
    return inDevice_ ;
}

template< class T >
uint8_t CLImage2DArray< T >::pixelSize() const
{
    return ( channelOrder_ == FRAME_CHANNEL_ORDER::ORDER_RGBA )?
                sizeof( T ) * 4 : sizeof( T );
}


template< class T >
uint8_t CLImage2DArray< T >::channelsInPixel() const
{
    return ( channelOrder_ == FRAME_CHANNEL_ORDER::ORDER_RGBA )?
                 4 : 1;
}

template< class T >
void CLImage2DArray< T >::releaseDeviceData_()
{
    if( deviceData_ != NULL )
        clReleaseMemObject( deviceData_ );

    inDevice_ = false ;
    deviceData_ = 0 ;
}


}
}

#include "CLImage2DArray.ipp"
