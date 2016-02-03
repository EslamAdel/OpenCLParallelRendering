#ifndef CLIMAGE_H
#define CLIMAGE_H
#include <Headers.h>
#include <QPixmap>
#include <Typedefs.hh>

enum IMAGE_PRECISION
{
    IMAGE_CL_UNSIGNED_INT8,
    IMAGE_CL_UNSIGNED_INT16,
    IMAGE_CL_UNSIGNED_INT32,
    IMAGE_CL_HALF_FLOAT,
    IMAGE_CL_FLOAT
};

template< class T >
class CLFrame
{
public:

    /**
     * @brief CLVolume
     * @param volume
     * @param precision
     */
    CLFrame(  uint* &image ,
              const Dimensions2D &dimensions ,
              const Coordinates3D &center );


    ~CLFrame() ;

public:

    /**
     * @brief createDeviceImage
     * creates empty device image.
     * @param context
     * @return
     */
    cl_mem createDeviceImage( cl_context context );



    void writeDeviceImage( cl_command_queue cmdQueue );

    void readDeviceImage( cl_command_queue cmdQueue );

    uint *getHostImage() const;

    QPixmap &getFramePixmap() ;

    void setHostImage( uint * &image );

    cl_mem getDeviceImage() const;


private:

    uint *&hostImage_;

    uchar *rgbaFrame_;

    QPixmap frame_ ;

    cl_mem deviceImage_ ;

    const Dimensions2D &dimensions_ ;

    const Coordinates3D &center_ ;

    cl_image_format imageFormat_ ;

};


typedef CLFrame< uint > CLFrame32 ;

#endif // CLFrame_H
