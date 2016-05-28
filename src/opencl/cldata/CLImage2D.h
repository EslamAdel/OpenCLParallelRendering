#ifndef CLIMAGE2D_H
#define CLIMAGE2D_H

#include "CLFrame.h"


// Forward declaration
template< class T > class SerializableFrame ;


namespace clparen {
namespace CLData {


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
    CLImage2D(
            const Dimensions2D dimensions ,
            const FRAME_CHANNEL_ORDER channelOrder =
            FRAME_CHANNEL_ORDER::ORDER_DEFAULT );

    CLImage2D( );

    /**
     * @brief createDeviceData
     * @param context
     * @param flags
     */
    virtual void createDeviceData(
            cl_context context ,
            const cl_mem_flags flags
            = CL_MEM_HOST_READ_ONLY | CL_MEM_WRITE_ONLY  ) Q_DECL_OVERRIDE;

    /**
     * @brief writeDeviceData
     * @param cmdQueue
     * @param blocking
     */
    virtual void writeDeviceData(  cl_command_queue cmdQueue ,
                                   const cl_bool blocking ) Q_DECL_OVERRIDE;

    /**
     * @brief readDeviceData
     * @param cmdQueue
     * @param blocking
     */
    virtual void readDeviceData( cl_command_queue cmdQueue ,
                                 const cl_bool blocking ) Q_DECL_OVERRIDE;

    /**
     * @brief readOtherDeviceData
     * @param sourceCmdQueue
     * @param sourceFrame
     * @param blocking
     */
    virtual void readOtherDeviceData( cl_command_queue sourceCmdQueue ,
                                      const CLFrame< T > &sourceFrame ,
                                      const cl_bool blocking ) Q_DECL_OVERRIDE;

private:


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


}
}


#endif // CLIMAGE2D_H
