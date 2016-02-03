#ifndef CLIMAGE_H
#define CLIMAGE_H
#include <Headers.h>
#include <QPixmap>
#include <Typedefs.hh>

template< class T >
class CLFrame
{
public:

    /**
     * @brief CLVolume
     * @param volume
     * @param precision
     */
    CLFrame( const Dimensions2D dimensions ,
             T* data = nullptr );


    ~CLFrame() ;

public:

    /**
     * @brief createDeviceImage
     * creates empty device image.
     * @param context
     * @return
     */

    //TODO : new class FrameImage that inherit from this class.
    virtual void createDeviceData( cl_context context );


    virtual void writeDeviceData(  cl_command_queue cmdQueue ,
                                   const cl_bool blocking );

    virtual void readDeviceData( cl_command_queue cmdQueue ,
                                 const cl_bool blocking );

    T *getHostData() const;

    QPixmap &getFramePixmap() ;

    void setHostData( T *data );

    cl_mem getDeviceData() const;


private:

    T *hostData_;

    cl_mem deviceData_ ;

    uchar *rgbaFrame_;

    QPixmap frame_ ;

    const Dimensions2D dimensions_ ;

    //cl_image_format imageFormat_ ;

};


typedef CLFrame< uint > CLFrame32 ;

#endif // CLFrame_H
