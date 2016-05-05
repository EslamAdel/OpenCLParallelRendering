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

    Q_DISABLE_COPY( CLFrame )


public:

    /**
     * @brief CLVolume
     * @param volume
     * @param precision
     */
    CLFrame( const Dimensions2D dimensions  );

    /**
     * @brief CLFrame
     */
    CLFrame( );

    ~CLFrame( ) ;

public:

    /**
     * @brief createDeviceImage
     * creates empty device image.
     * @param context
     * @return
     */

    //TODO : new class FrameImage that inherit from this class.
    virtual void createDeviceData( cl_context context ,
                                   const cl_mem_flags flags
                                   = CL_MEM_READ_WRITE );


    /**
     * @brief writeDeviceData
     * @param cmdQueue
     * @param blocking
     */
    virtual void writeDeviceData(  cl_command_queue cmdQueue ,
                                   const cl_bool blocking );

    /**
     * @brief readDeviceData
     * @param cmdQueue
     * @param blocking
     */
    virtual void readDeviceData( cl_command_queue cmdQueue ,
                                 const cl_bool blocking );

    /**
     * @brief readOtherDeviceData
     * @param sourceCmdQueue
     * @param sourceFrame
     * @param blocking
     */
    virtual void readOtherDeviceData( cl_command_queue sourceCmdQueue ,
                                      const CLFrame< T > &sourceFrame ,
                                      const cl_bool blocking );

    /**
     * @brief copyDeviceData
     * @param cmdQueue
     * @param frame
     * @param blocking
     */
    virtual void copyDeviceData( cl_command_queue cmdQueue ,
                                 const CLFrame< T > &frame ,
                                 const cl_bool blocking );


    /**
     * @brief getHostData
     * @return
     */
    virtual T *getHostData() const;

    /**
     * @brief getFramePixmap
     * @return
     */
    virtual QPixmap &getFramePixmap() ;

    /**
     * @brief copyHostData
     * @param data
     */
    virtual void copyHostData( const T *data );

    /**
     * @brief copyHostData
     * @param sourceFrame
     */
    virtual void copyHostData( const CLFrame< T > &sourceFrame );

    /**
     * @brief setHostData
     * @param data
     * @param copy
     */
    virtual void setHostData( T *data ,
                              bool deepCopy = true );

    /**
     * @brief getDeviceData
     * @return
     */
    virtual cl_mem getDeviceData() const;

    /**
     * @brief getFrameDimensions
     * @return
     */
    const Dimensions2D &getFrameDimensions() const;

    /**
     * @brief getContext
     * @return
     */
    const cl_context getContext() const ;

    /**
     * @brief isInDevice
     * @return
     */
    bool isInDevice() const ;

    /**
     * @brief inSameContext
     * @param frame
     * @return
     */
    bool inSameContext( const CLFrame< T > &frame ) const ;


    /**
     * @brief dimensionsDefined
     * @return
     */
    bool dimensionsDefined( ) const ;

protected:
    /**
     * @brief releaseDeviceData_
     */
    void releaseDeviceData_();


    /**
     * @brief initializeHostBuffers_
     */
    void initializeHostBuffers_( );

    //Temporary solution as namesapce SystemUtilites cannot be found
    //by compiler.
    /**
     * @brief convertColorToRGBA_
     * @param Color
     * @param r
     * @param g
     * @param b
     * @param a
     */
    void convertColorToRGBA_( uint Color,
                              uint8_t &r , uint8_t &g,
                              uint8_t &b , uint8_t &a );
protected:

    /**
     * @brief hostData_
     */
    T *hostData_;

    /**
     * @brief deviceData_
     */
    cl_mem deviceData_ ;

    /**
     * @brief pixmapData_
     */
    uchar *pixmapData_;

    /**
     * @brief frame_
     */
    QPixmap frame_ ;

    /**
     * @brief dimensions_
     */
    Dimensions2D dimensions_ ;

    /**
     * @brief context_
     */
    cl_context context_ ;

    //flags
    /**
     * @brief pixmapSynchronized_
     */
    bool pixmapSynchronized_ ;

    /**
     * @brief inDevice_
     */
    bool inDevice_ ;

    /**
     * @brief dimensionsDefined_
     */
    bool dimensionsDefined_ ;

};


typedef CLFrame< uint > CLFrame32 ;


#endif // CLFrame_H
