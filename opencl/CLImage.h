#ifndef CLIMAGE_H
#define CLIMAGE_H

#include "Image.h"
#include <Headers.h>



enum IMAGE_PRECISION
{
    IMAGE_CL_UNSIGNED_INT8,
    IMAGE_CL_UNSIGNED_INT16,
    IMAGE_CL_UNSIGNED_INT32,
    IMAGE_CL_HALF_FLOAT,
    IMAGE_CL_FLOAT
};

template< class T >
class CLImage
{
public:

    /**
     * @brief CLVolume
     * @param volume
     * @param precision
     */
    CLImage(  Image< T >* image, const IMAGE_PRECISION precision );

    ~CLImage() ;

public:

    /**
     * @brief createDeviceVolume
     * @param context
     * @return
     */
    cl_mem createDeviceImage( cl_context context );

    void writeDeviceImage( cl_command_queue cmdQueue );

    void readDeviceImage( cl_command_queue cmdQueue );

    Image< T > *getHostImage() const;

    cl_mem getDeviceImage() const;


private:

    /**
     * @brief volume_
     */
    Image< T >* hostImage_;

    cl_mem deviceImage_ ;


    const uint64_t width_;

    const uint64_t height_;

    /**
     * @brief precision_
     */
    const IMAGE_PRECISION precision_;
};
#endif // CLIMAGE_H
