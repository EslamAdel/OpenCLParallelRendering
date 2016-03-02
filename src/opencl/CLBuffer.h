#ifndef CLBUFFER_H
#define CLBUFFER_H

#include "oclHWDL.h"
#include <QVector>
#include <QDataStream>


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
public:


    CLBuffer();

    CLBuffer( const u_int64_t size );


    ~CLBuffer( ) ;

public:
    //TODO : new class FrameImage that inherit from this class.
    virtual void createDeviceData( cl_context context );


    virtual void writeDeviceData(  cl_command_queue cmdQueue ,
                                   const cl_bool blocking );

    virtual void readDeviceData( cl_command_queue cmdQueue ,
                                 const cl_bool blocking );

    virtual void resize( u_int64_t newSize );

    virtual T *getHostData() const;

    virtual void setHostData( const T *data );

    virtual void setHostData( const QVector< T > &vector );

    virtual cl_mem getDeviceData() const;

    const size_t size() const;

    const cl_context getContext() const ;

    bool inDevice() const ;

private:
    void releaseDeviceData_();

    friend QDataStream& operator<< <>(  QDataStream& stream ,
                                        const CLBuffer< T > &buffer );

    friend QDataStream& operator>> <>( QDataStream& stream ,
                                       CLBuffer< T > &buffer );

private:
    u_int64_t size_ ;

    cl_context context_ ;

    cl_mem deviceData_ ;

    T *hostData_ ;

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




#endif // CLBUFFER_H
