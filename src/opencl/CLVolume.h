#ifndef RENDERING_CLVOLUME_H
#define RENDERING_CLVOLUME_H

#include <Headers.h>
#include <Volume.h>

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
public:

    /**
     * @brief CLVolume
     * @param volume
     * @param precision
     */
    CLVolume( const Volume< T >* volume, const VOLUME_PRECISION precision );

public:

    /**
     * @brief createDeviceVolume
     * @param context
     * @return
     */
    cl_mem createDeviceVolume( cl_context context );

private:

    /**
     * @brief volume_
     */
    const Volume< T >* volume_;

    /**
     * @brief precision_
     */
    const VOLUME_PRECISION precision_;
};

#endif // RENDERING_CLVOLUME_H