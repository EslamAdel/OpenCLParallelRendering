#ifndef CLIMAGE2DARRAY_H
#define CLIMAGE2DARRAY_H

#include <oclHWDL.h>

template< class T >
class CLImage2DArray
{
public:
    CLImage2DArray( const uint width , const uint height ,
                    const uint arraySize ,
                    cl_channel_order channelOrder = CL_INTENSITY );

    void createDeviceData( cl_context context );

    void setFrameData( const uint index , T *data );

    void loadFrameDataToDevice( const uint index ,
                                cl_command_queue commandQueue,
                                cl_bool blocking ) ;

    cl_mem getDeviceData() const ;

private:
    const uint width_ ;
    const uint height_ ;
    const uint arraySize_;

    cl_mem deviceData_ ;

    cl_image_format imageFormat_ ;

    std::vector< T* > framesData_ ;
    std::vector< bool > framesSet_ ;
};


typedef CLImage2DArray< uint > CLImage2DArray32 ;

#endif // CLIMAGE2DARRAY_H
