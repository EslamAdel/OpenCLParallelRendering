#ifndef CLTRANSFERFUNCTION_H
#define CLTRANSFERFUNCTION_H

#include "CLImage2D.h"

class CLTransferFunction : public CLImage2D< float >
{
public:
    CLTransferFunction(  const uint length ,
                         const float *transferFunctionData  );


    void createDeviceData( cl_context context )  ;

    void writeDeviceData(  cl_command_queue cmdQueue ,
                           const cl_bool blocking ) override ;

private:
    const uint length_ ;
};

#endif // CLTRANSFERFUNCTION_H
