#ifndef RENDERING_CLVOLUME_H
#define RENDERING_CLVOLUME_H

#include <Headers.h>
#include "Volume.h"
#include <QMutex>
#include <QMutexLocker>

namespace clparen {
namespace clData {

enum VOLUME_PRECISION
{
    VOLUME_CL_UNSIGNED_INT8,
    VOLUME_CL_UNSIGNED_INT16,
    VOLUME_CL_UNSIGNED_INT32,
    VOLUME_CL_HALF_FLOAT,
    VOLUME_CL_FLOAT
};



template< class T >
class CLVolume
{
    Q_DISABLE_COPY( CLVolume )
public:

    /**
     * @brief CLVolume
     * @param volume
     * @param precision
     */
    CLVolume( Volume< T >* volume, const VOLUME_PRECISION precision );

public:

    /**
     * @brief createDeviceVolume
     * @param context
     * @return
     */
    cl_mem createDeviceVolume( cl_context context );


    /**
     * @brief getDeviceData
     * @return
     */
    cl_mem getDeviceData( ) const ;

    /**
     * @brief writeDeviceData
     * @param cmdQueue
     * @param blocking
     */
    void writeDeviceData( cl_command_queue cmdQueue ,
                          const cl_bool blocking ) ;

    /**
     * @brief copyHostData
     * @param data
     */
    void copyHostData( const T *data );

    /**
     * @brief copyHostData
     * @param brickParameters
     */
    void copyHostData( const BrickParameters< T > &brickParameters );
private:

    /**
     * @brief volume_
     */
    Volume< T >* volume_;

    /**
     * @brief precision_
     */
    const VOLUME_PRECISION precision_;

    /**
     * @brief imageDescriptor_
     */
    cl_image_desc imageDescriptor_ ;

    /**
     * @brief imageFormat_
     */
    cl_image_format imageFormat_ ;

    /**
     * @brief flags_
     */
    cl_mem_flags flags_ ;

    /**
     * @brief deviceData_
     */
    cl_mem deviceData_ ;

    /**
     * @brief hostDataMutex_
     */
    QMutex hostDataMutex_ ;
};


}
}


#endif // RENDERING_CLVOLUME_H
