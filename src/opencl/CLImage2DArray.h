#ifndef CLIMAGE2DARRAY_H
#define CLIMAGE2DARRAY_H

#include <oclHWDL.h>
#include <QVector>
#include "CLFrame.h"

template< class T >
class CLImage2DArray
{
public:


    CLImage2DArray( const uint width , const uint height ,
                    const uint arraySize ,
                    const cl_channel_order channelOrder = CL_RGBA ,
                    const cl_channel_type channelType = CL_UNORM_INT8  );

    ~CLImage2DArray();

    void createDeviceData( cl_context context );

    void setFrameData( const uint index , T *data );

    void loadFrameDataToDevice( const uint index ,
                                cl_command_queue commandQueue,
                                cl_bool blocking ) ;

    cl_mem getDeviceData() const ;

    /**
     * @brief getFrameDepth
     * @return
     */
    float getFrameDepth();

    void resize( const uint newArraySize , cl_context context );

    size_t size() const ;


    void readOtherDeviceData( cl_command_queue cmdQueue ,
                              const uint index ,
                              const CLFrame< T > &source ,
                              cl_bool blocking ) ;

    bool inDevice() const ;

private :
    void releaseDeviceData_();


private:
    const uint width_    ;
    const uint height_   ;
    uint arraySize_ ;
    cl_mem deviceData_ ;

    cl_image_format imageFormat_ ;
    float frameDepth_;
    QVector< T* > framesData_ ;
    QVector< bool > framesSet_ ;

    cl_context context_ ;
    bool inDevice_ ;
};


typedef CLImage2DArray< uint > CLImage2DArray32 ;

#endif // CLIMAGE2DARRAY_H
