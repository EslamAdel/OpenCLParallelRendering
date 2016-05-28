#ifndef CLTRANSFERFUNCTION_H
#define CLTRANSFERFUNCTION_H

#include "CLImage2D.h"

namespace clparen {
namespace CLData {


class CLTransferFunction : public CLImage2D< float >
{
    Q_DISABLE_COPY( CLTransferFunction )

public:
    /**
     * @brief CLTransferFunction
     * @param length
     * @param transferFunctionData
     */
    CLTransferFunction(  const uint length ,
                         const float *transferFunctionData  );

    /**
     * @brief createDeviceData
     * @param context
     */
    void createDeviceData( cl_context context ) Q_DECL_OVERRIDE ;

    /**
     * @brief writeDeviceData
     * @param cmdQueue
     * @param blocking
     */
    void writeDeviceData(  cl_command_queue cmdQueue ,
                           const cl_bool blocking ) Q_DECL_OVERRIDE ;

private:
    /**
     * @brief length_
     */
    const uint length_ ;
};



}
}


#endif // CLTRANSFERFUNCTION_H
