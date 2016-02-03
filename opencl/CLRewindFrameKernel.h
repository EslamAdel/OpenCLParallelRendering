#ifndef CLREWINDFRAMEKERNEL_H
#define CLREWINDFRAMEKERNEL_H

#include "Headers.hh"
#include <oclHWDL/oclHWDL.h>
#include <QVector>


class CLRewindFrameKernel
{
public:

    enum KernelArguments {
        Frame = 0 ,
        FrameWidth ,
        FrameHeight
    };

    CLRewindFrameKernel(const cl_context clContext ,
                        const std::string kernelDirectory = ".",
                        const std::string kernelFile = "xray_compositing.cl" ,
                        const std::string kernelName = "rewind_image2d");

    void setFrame(const cl_mem frameImage2D );

    void setFrameWidth( const uint width );

    void setFrameHeight( const uint height );

    void releaseKernel( );

//    std::string getKernelDirectory( ) const;

//    std::string getKernelFile( ) const;

//    oclHWDL::KernelContext *getKernelContext( ) const;

    cl_kernel getKernelObject( ) const;


private:

    void buildKernel_( const std::string extraBuildOptions = "" );
    void retrieveKernelObject_( );

    cl_context clContext_;
    oclHWDL::KernelContext* kernelContext_;
    cl_kernel kernelObject_;

    const std::string kernelName_;
    const std::string kernelDirectory_;
    const std::string kernelFile_;


};

#endif // CLREWINDFRAMEKERNEL_H
