#ifndef CLIMAGE2D_H
#define CLIMAGE2D_H

#include "CLFrame.h"



// Forward declaration
template< class T > class SerializableFrame ;


template< class T >
class CLImage2D : public CLFrame< T >
{


public:
    /**
     * @brief CLImage2D
     * @param dimensions
     * @param channelOrder
     * @param channelType
     */
    CLImage2D(  const Dimensions2D dimensions ,
                const cl_channel_order channelOrder = CL_RGBA ,
                const cl_channel_type channelType = CL_UNORM_INT8  );

    CLImage2D( );

    /**
     * @brief createDeviceData
     * @param context
     * @param flags
     */
    virtual void createDeviceData( cl_context context ,
                                   const cl_mem_flags flags
                                   = CL_MEM_WRITE_ONLY |
                                     CL_MEM_HOST_READ_ONLY  ) override;

    /**
     * @brief writeDeviceData
     * @param cmdQueue
     * @param blocking
     */
    virtual void writeDeviceData(  cl_command_queue cmdQueue ,
                                   const cl_bool blocking ) override;

    /**
     * @brief readDeviceData
     * @param cmdQueue
     * @param blocking
     */
    virtual void readDeviceData( cl_command_queue cmdQueue ,
                                 const cl_bool blocking ) override;

    /**
     * @brief readOtherDeviceData
     * @param sourceCmdQueue
     * @param sourceFrame
     * @param blocking
     */
    virtual void readOtherDeviceData( cl_command_queue sourceCmdQueue ,
                                      const CLFrame< T > &sourceFrame ,
                                      const cl_bool blocking ) override;



protected :
    /**
     * @brief imageFormat_
     */
    cl_image_format imageFormat_ ;

    /**
     * @brief imageDescriptor_
     */
    cl_image_desc   imageDescriptor_ ;

private:
    friend class SerializableFrame< T > ;


};


#endif // CLIMAGE2D_H
