#ifndef CLIMAGE2DARRAY_H
#define CLIMAGE2DARRAY_H

#include <oclHWDL.h>

template< class T >
class CLImage2DArray
{
public:


    CLImage2DArray( const uint width , const uint height ,
                    const uint arraySize ,
                    const cl_channel_order channelOrder = CL_INTENSITY ,
                    const cl_channel_type channelType = CL_UNORM_INT8 ,
                    const float depth = 0);

    void createDeviceData( cl_context context );

    void setFrameData( const uint index , T *data ,float depth);

    void loadFrameDataToDevice( const uint index ,
                                cl_command_queue commandQueue,
                                cl_bool blocking ) ;

    cl_mem getDeviceData() const ;

    /**
     * @brief getFrameDepth
     * @return
     */
    float getFrameDepth();

private:
    const uint width_ ;
    const uint height_ ;
    const uint arraySize_;

    cl_mem deviceData_ ;

    cl_image_format imageFormat_ ;
    float frameDepth_;
    std::vector< T* > framesData_ ;
    std::vector< bool > framesSet_ ;
};


typedef CLImage2DArray< uint > CLImage2DArray32 ;

#endif // CLIMAGE2DARRAY_H
