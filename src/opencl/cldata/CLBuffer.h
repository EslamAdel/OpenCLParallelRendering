#ifndef CLBUFFER_H
#define CLBUFFER_H

#include "oclHWDL.h"
#include <QVector>
#include <QDataStream>


namespace clparen {
namespace clData {


// Forward declarations
template<class T> class CLBuffer;

template<class T> QDataStream&
operator<<( QDataStream& stream ,
            const CLBuffer< T > &buffer );

template<class T> QDataStream&
operator>>( QDataStream& stream ,
            CLBuffer< T > &buffer );



template< class T >
class CLBuffer
{
    Q_DISABLE_COPY( CLBuffer )

public:

    /**
     * @brief CLBuffer
     */
    CLBuffer();

    /**
     * @brief CLBuffer
     * @param size
     */
    CLBuffer( const u_int64_t size );


    ~CLBuffer( ) ;

public:
    /**
     * @brief createDeviceData
     * @param context
     */
    virtual void createDeviceData( cl_context context );


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
     * @brief resize
     * @param newSize
     */
    virtual void resize( u_int64_t newSize );

    /**
     * @brief getHostData
     * @return
     */
    virtual T *getHostData() const;

    /**
     * @brief setHostData
     * @param data
     */
    virtual void setHostData( const T *data );

    /**
     * @brief setHostData
     * @param vector
     */
    virtual void setHostData( const QVector< T > &vector );

    /**
     * @brief getDeviceData
     * @return
     */
    virtual cl_mem getDeviceData() const;

    /**
     * @brief size
     * @return
     */
    const size_t size() const;

    /**
     * @brief getContext
     * @return
     */
    const cl_context getContext() const ;

    /**
     * @brief inDevice
     * @return
     */
    bool inDevice() const ;

private:

    /**
     * @brief releaseDeviceData_
     */
    void releaseDeviceData_();


    friend QDataStream& operator<< <>(  QDataStream& stream ,
                                        const CLBuffer< T > &buffer );

    friend QDataStream& operator>> <>( QDataStream& stream ,
                                       CLBuffer< T > &buffer );

private:
    /**
     * @brief size_
     */
    u_int64_t size_ ;

    /**
     * @brief context_
     */
    cl_context context_ ;

    /**
     * @brief deviceData_
     */
    cl_mem deviceData_ ;

    /**
     * @brief hostData_
     */
    T *hostData_ ;

    /**
     * @brief inDevice_
     */
    bool inDevice_ ;

};


template< class T >
QDataStream& operator<<( QDataStream& stream ,
                         const CLBuffer< T > &buffer );

template< class T >
QDataStream& operator>>( QDataStream& stream ,
                         CLBuffer< T > &buffer );


//#define REGISTER_STREAM_OPERATOR()\
//    do{ \
//    qRegisterMetaTypeStreamOperators< CLBuffer< uint > >\
//    ("CLBuffer< uint >") ;\
//    }while( 0 )



}
}
#endif // CLBUFFER_H
