#ifndef CLIMAGE2DARRAY_H
#define CLIMAGE2DARRAY_H

#include <oclHWDL.h>
#include <QVector>
#include "CLImage2D.h"

template< class T >
class CLImage2DArray
{
    Q_DISABLE_COPY( CLImage2DArray )
public:


    /**
     * @brief CLImage2DArray
     * @param width
     * @param height
     * @param arraySize
     * @param channelOrder
     * @param channelType
     */
    CLImage2DArray( const uint width , const uint height ,
                    const uint arraySize ,
                    const cl_channel_order channelOrder = CL_RGBA ,
                    const cl_channel_type channelType = CL_UNORM_INT8  );

    ~CLImage2DArray();

    /**
     * @brief createDeviceData
     * @param context
     */
    void createDeviceData( cl_context context );

    /**
     * @brief setFrameData
     * @param index
     * @param data
     */
    void setFrameData( const uint index , const T *data );

    /**
     * @brief loadFrameDataToDevice
     * @param index
     * @param commandQueue
     * @param blocking
     */
    void loadFrameDataToDevice( const uint index ,
                                cl_command_queue commandQueue,
                                cl_bool blocking ) ;

    /**
     * @brief getDeviceData
     * @return
     */
    cl_mem getDeviceData() const ;

    /**
     * @brief getFrameDepth
     * @return
     */
    float getFrameDepth();

    /**
     * @brief resize
     * @param newArraySize
     * @param context
     */
    void resize( const uint newArraySize , cl_context context );

    /**
     * @brief size
     * @return
     */
    size_t size() const ;


    /**
     * @brief readOtherDeviceData
     * @param cmdQueue
     * @param index
     * @param source
     * @param blocking
     */
    void readOtherDeviceData( cl_command_queue cmdQueue ,
                              const uint index ,
                              const CLImage2D< T > &source ,
                              cl_bool blocking ) ;

    /**
     * @brief inDevice
     * @return
     */
    bool inDevice() const ;

private :
    /**
     * @brief releaseDeviceData_
     */
    void releaseDeviceData_();


private:
    /**
     * @brief width_
     */
    const uint width_    ;

    /**
     * @brief height_
     */
    const uint height_   ;

    /**
     * @brief arraySize_
     */
    uint arraySize_ ;

    /**
     * @brief deviceData_
     */
    cl_mem deviceData_ ;

    /**
     * @brief imageFormat_
     */
    cl_image_format imageFormat_ ;

    /**
     * @brief imageDescriptor_
     */
    cl_image_desc imageDescriptor_ ;

    /**
     * @brief frameDepth_
     */
    float frameDepth_;

    /**
     * @brief framesData_
     */
    QVector< T* > framesData_ ;

    /**
     * @brief context_
     */
    cl_context context_ ;

    /**
     * @brief inDevice_
     */
    bool inDevice_ ;
};


typedef CLImage2DArray< uint > CLImage2DArray32 ;

#endif // CLIMAGE2DARRAY_H
