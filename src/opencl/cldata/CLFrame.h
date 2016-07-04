#ifndef CLIMAGE_H
#define CLIMAGE_H
#include <QPixmap>
#include "Headers.h"
#include "Typedefs.hh"
#include <oclHWDL/ErrorHandler.h>
#include "Utilities.h"
#include "Image.h"
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>

namespace clparen {
namespace CLData {

enum FRAME_CHANNEL_ORDER
{
    ORDER_INTENSITY ,
    ORDER_RGBA ,
    ORDER_DEFAULT
};

enum FRAME_CHANNEL_TYPE
{
    FRAME_CL_UNSIGNED_INT8,
    FRAME_CL_UNSIGNED_INT16,
    FRAME_CL_UNSIGNED_INT32,
    FRAME_CL_HALF_FLOAT,
    FRAME_CL_FLOAT,
};


// Forward declaration
template< class T > class CLImage2D;

template< class T >
class CLFrame
{

    Q_DISABLE_COPY( CLFrame )

protected:
     friend class CLImage2D< T > ;

public:

    /**
     * @brief CLVolume
     * @param volume
     * @param precision
     */
    CLFrame( const Dimensions2D dimensions ,
             const FRAME_CHANNEL_ORDER channelOrder =
            FRAME_CHANNEL_ORDER::ORDER_DEFAULT );

    /**
     * @brief CLFrame
     */
    CLFrame( const FRAME_CHANNEL_ORDER channelOrder =
            FRAME_CHANNEL_ORDER::ORDER_DEFAULT );


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
    QPixmap &getFramePixmap() ;

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
     * @brief copyHostData
     * @param sourceFrame
     */
    virtual void copyHostData( const Image< T > &sourceFrame );

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


    /**
     * @brief frameChannelPrecision
     * @return
     */
    static FRAME_CHANNEL_TYPE frameChannelType( );


    /**
     * @brief clChannelType
     * @return
     */
    static cl_channel_type clChannelType( );


    /**
     * @brief defaultChannelOrder
     * @return
     */
    static FRAME_CHANNEL_ORDER defaultChannelOrder( );

    /**
     * @brief clChannelOrder
     * @param order
     * @return
     */
    static cl_channel_order clChannelOrder( FRAME_CHANNEL_ORDER order );



    /**
     * @brief checkChannelConflict_
     */
    static bool isChannelConflict( FRAME_CHANNEL_ORDER order );

    /**
     * @brief frameChannelOrder
     * @return
     */
    FRAME_CHANNEL_ORDER channelOrder() const;

    /**
     * @brief pixelSize
     * @return
     */
    uint8_t pixelSize( ) const ;

    /**
     * @brief channelsInPixel
     * @return
     */
    uint8_t channelsInPixel( ) const ;


    /**
     * @brief getRegion
     * @return
     */
    const Dimensions2D &getRegion() const;

    /**
     * @brief getOffset
     * @return
     */
    const Dimensions2D &getOffset() const;


    /**
     * @brief setRegion
     * @param offset
     * @param region
     */
    void setRegion( const Dimensions2D  &offset ,
                    const Dimensions2D &region );

    /**
     * @brief getTransferTime
     * @return
     */
    float getTransferTime() const;


    /**
     * @brief checksum
     * @return
     */
    T checksum() const;
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


    /**
     * @brief calculateTransferTime_
     * @return
     */
    cl_int evaluateTransferTime_() const;
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
    QPixmap *pixmap_ ;

    /**
     * @brief dimensions_
     */
    Dimensions2D dimensions_ ;

    /**
     * @brief region_
     */
    Dimensions2D region_ ;

    /**
     * @brief offset_
     */
    Dimensions2D offset_ ;

    /**
     * @brief context_
     */
    cl_context context_ ;

    /**
     * @brief transferEvent_
     */
    mutable cl_event clTransferEvent_ ;

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

    /**
     * @brief channelOrder_
     */
    const FRAME_CHANNEL_ORDER channelOrder_ ;

    /**
     * @brief readWriteLock_
     */
    mutable QReadWriteLock regionLock_;

    /**
     * @brief transferTime_
     */
    mutable float transferTime_ ;

    /**
     * @brief transferTimeLock_
     */
    mutable QReadWriteLock transferTimeLock_;
};


}
}

Q_DECLARE_METATYPE( clparen::CLData::CLFrame< uint8_t >* )
Q_DECLARE_METATYPE( clparen::CLData::CLFrame< uint16_t >* )
Q_DECLARE_METATYPE( clparen::CLData::CLFrame< uint32_t >* )
Q_DECLARE_METATYPE( clparen::CLData::CLFrame< half >* )
Q_DECLARE_METATYPE( clparen::CLData::CLFrame< float >* )


#endif // CLFrame_H
