#ifndef CLIMAGE_H
#define CLIMAGE_H
#include <Headers.h>
#include <QPixmap>
#include <Typedefs.hh>
#include <oclHWDL/ErrorHandler.h>
#include "Utilities.h"


namespace SystemUtilities {}
using namespace SystemUtilities;

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

    virtual void readOtherDeviceData( cl_command_queue sourceCmdQueue ,
                                      const CLFrame< T > &sourceFrame ,
                                      const cl_bool blocking );

    virtual void copyDeviceData( cl_command_queue cmdQueue ,
                                 const CLFrame< T > &frame ,
                                 const cl_bool blocking );


    virtual T *getHostData() const;

    virtual QPixmap &getFramePixmap() ;

    virtual void setHostData( T *data );

    virtual void copyHostData( T *data );

    virtual void copyHostData( CLFrame< T > &sourceFrame );

    virtual cl_mem getDeviceData() const;

    const Dimensions2D &getFrameDimensions() const;

    const cl_context getContext() const ;

    bool isInDevice() const ;

    bool inSameContext( const CLFrame<T> &frame ) const ;


private:
    void releaseDeviceData_();

    //Temporary solution as namesapce SystemUtilites cannot be found
    //by compiler.
    void convertColorToRGBA_( uint Color,
                             uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a );
protected:

    T *hostData_;

    cl_mem deviceData_ ;

    uchar *rgbaFrame_;

    QPixmap frame_ ;

    const Dimensions2D dimensions_ ;

    cl_context context_ ;

    //flags
    bool pixmapSynchronized_ ;
    bool inDevice_ ;
};


typedef CLFrame< uint > CLFrame32 ;

#endif // CLFrame_H
