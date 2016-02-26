#ifndef CLBUFFER_H
#define CLBUFFER_H

#include "oclHWDL.h"
#include <QVector>

template< class T >
class CLBuffer
{
public:

    CLBuffer( const u_int64_t size );


    ~CLBuffer() ;

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


private:
    u_int64_t size_ ;

    cl_context context_ ;

    cl_mem deviceData_ ;

    T *hostData_ ;

    bool inDevice_ ;

};

#endif // CLBUFFER_H
