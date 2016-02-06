#include "CLImage2DArray.h"
#include "Logger.h"

template< class T >
CLImage2DArray< T >::CLImage2DArray( const uint width ,
                                     const uint height ,
                                     const uint arraySize ,
                                     const cl_channel_order channelOrder ,
                                     const cl_channel_type channelType ,
                                     const float depth)
    : width_( width ) ,
      height_( height ) ,
      arraySize_( arraySize ) ,
      frameDepth_(depth)
{

    if( typeid( T ) != typeid( uint ) )
        LOG_ERROR("Not supperted type!");

    for( auto i = 0 ; i < arraySize ; i++ )
    {
        framesData_.push_back( new T );
        framesSet_.push_back( false );
    }

    imageFormat_.image_channel_order = channelOrder ;
    imageFormat_.image_channel_data_type = channelType ;
}


template< class T >
void CLImage2DArray< T >::createDeviceData( cl_context context )
{

    cl_int error = CL_SUCCESS;

    deviceData_ =
            clCreateImage3D( context , CL_MEM_READ_WRITE ,
                             &imageFormat_ , width_ , height_ ,
                             arraySize_ , 0 , 0 , NULL , &error ) ;


    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("OpenCL Error!");
    }


}

template< class T >
void CLImage2DArray< T >::setFrameData( const uint index , T *data ,float depth)
{
    if( index >= arraySize_ )
        LOG_ERROR("Out of range index!");

    if( data != framesData_[ index ])
    {
        delete framesData_[ index ];
        framesData_[ index ] = data;
        framesSet_[ index ] = true ;
        frameDepth_ = depth;
    }
}


template< class T >
void CLImage2DArray< T >::loadFrameDataToDevice( const uint index ,
                                                 cl_command_queue  commandQueue ,
                                                 cl_bool blocking )
{
    if( ! framesSet_[ index ] )
        LOG_ERROR("Frame data has not been set!");

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






#include "CLImage2DArray.ipp"
