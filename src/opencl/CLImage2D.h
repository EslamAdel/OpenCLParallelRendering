#ifndef CLIMAGE2D_H
#define CLIMAGE2D_H

#include "CLFrame.h"

template< class T >
class CLImage2D : public CLFrame< T >
{


public:
    CLImage2D(  const Dimensions2D dimensions ,
                const cl_channel_order channelOrder = CL_RGBA ,
                const cl_channel_type channelType = CL_UNORM_INT8  );

    virtual void createDeviceData( cl_context context ) override;


    virtual void writeDeviceData(  cl_command_queue cmdQueue ,
                                   const cl_bool blocking ) override;

    virtual void readDeviceData( cl_command_queue cmdQueue ,
                                 const cl_bool blocking ) override;

    virtual void readOtherDeviceData( cl_command_queue sourceCmdQueue ,
                                      const CLFrame< T > &sourceFrame ,
                                      const cl_bool blocking ) override ;



private :
    cl_image_format imageFormat_ ;
};


typedef CLImage2D< uint > CLImage32 ;

#endif // CLIMAGE2D_H
