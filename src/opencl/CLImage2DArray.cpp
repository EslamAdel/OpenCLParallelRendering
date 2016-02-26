#include "CLImage2DArray.h"
#include "Logger.h"
#include <typeinfo>

template< class T >
CLImage2DArray< T >::CLImage2DArray( const uint width ,
                                     const uint height ,
                                     const uint arraySize ,
                                     const cl_channel_order channelOrder ,
                                     const cl_channel_type channelType  )
    : width_( width ) ,
      height_( height ) ,
      arraySize_( arraySize ) ,
      context_( NULL ) ,
      deviceData_( NULL )
{

    if( typeid( T ) != typeid( uint ) )
        LOG_ERROR("Not supperted type!");

    for( auto i = 0 ; i < arraySize ; i++ )
    {
        framesData_.push_back( new T[ width * height ] );
        framesSet_.push_back( false );
    }

    imageFormat_.image_channel_order = channelOrder ;
    imageFormat_.image_channel_data_type = channelType ;


    inDevice_ = false ;
}

template< class T >
CLImage2DArray< T >::~CLImage2DArray()
{
    if( inDevice_ )
        releaseDeviceData_();

    for( T* data : framesData_ )
    {
        delete data ;
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
            clCreateImage3D( context , CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY ,
                             &imageFormat_ , width_ , height_ ,
                             arraySize_ , 0 , 0 , NULL , &error ) ;


    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_DEBUG("Failed creating image3d: %dx%dx%d",
                  width_ , height_ , arraySize_ );
        LOG_ERROR("OpenCL Error!");
    }

    inDevice_ = true ;
}

template< class T >
void CLImage2DArray< T >::setFrameData( const uint index , T *data )
{
    if( index >= arraySize_ )
        LOG_ERROR("Out of range index!");

    if( data != framesData_[ index ])
    {
        std::copy( &data[0] ,
                &data[ width_ * height_ - 1 ] ,
                &framesData_[ index ][0] );

//        framesSet_[ index ] = true ;
    }
}


template< class T >
void CLImage2DArray< T >::loadFrameDataToDevice( const uint index ,
                                                 cl_command_queue  commandQueue ,
                                                 cl_bool blocking )
{
//    if( ! framesSet_[ index ] )
//        LOG_ERROR("Frame data has not been set!");

    if( index >= size() )
        LOG_ERROR("index exceeds the limit");


    cl_int error = CL_SUCCESS ;

    const size_t origin[] = { 0 , 0 , index } ;
    const size_t region[] = { width_ , height_ , 1 } ;

    error = clEnqueueWriteImage( commandQueue , deviceData_ , blocking ,
                                 origin , region , width_ * sizeof( uint ) ,
                                 width_ * height_ * sizeof( uint ) ,
                                 ( const void * ) framesData_[ index ] , 0 ,
                                 NULL , NULL ) ;

    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("OpenCL Error!");
    }


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
    releaseDeviceData_();
    arraySize_ = newArraySize ;

    int delta = newArraySize - framesData_.size();

    if( delta <= 0 )
        framesData_.resize( newArraySize );

    else
        for( uint i = 0 ; i < delta ; i++ )
            framesData_.push_back( new T[ width_ * height_ ]);


    createDeviceData( ( context_ != NULL )? context_ : context );

}

template< class T >
size_t CLImage2DArray< T >::size() const
{
    return arraySize_ ;
}

template< class T >
void CLImage2DArray< T >::readOtherDeviceData( cl_command_queue cmdQueue ,
                                               const uint index,
                                               const CLFrame<T> &source,
                                               cl_bool blocking)
{
    if( source.getFrameDimensions() != Dimensions2D( width_ , height_ ))
        LOG_ERROR("Dimensions mismatch!");

    static cl_int error = CL_SUCCESS;
    error = clEnqueueReadBuffer( cmdQueue ,
                                 source.getDeviceData() , blocking ,
                                 0 , width_ * height_ * sizeof(T) ,
                                 ( void * ) framesData_[ index ] ,
                                 0 , NULL , NULL);

    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("OpenCL Error!");
    }
}

template< class T >
void CLImage2DArray< T >::releaseDeviceData_()
{
    if( deviceData_ != NULL )
        clReleaseMemObject( deviceData_ );

    inDevice_ = false ;


}
#include "CLImage2DArray.ipp"
