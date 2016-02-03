#ifndef CLXRayCompositingKernel_H
#define CLXRayCompositingKernel_H


#include "Headers.hh"
#include <oclHWDL/oclHWDL.h>
#include <QVector>

class CLXRayCompositingKernel
{
public:
    enum KernelArguments {
        CollageFrame = 0 ,
        CollageWidth ,
        CollageHeight ,
        Frame ,
        FrameWidth ,
        FrameHeight,
        FrameCenterX ,
        FrameCenterY
    };

    CLXRayCompositingKernel( const cl_context clContext ,
                             const std::string kernelDirectory = ".",
                             const std::string kernelFile = "xray_compositing.cl" ,
                             const std::string kernelName = "xray_compositing");


    void setCollegeFrame( const cl_mem collageImage2D );

    void setCollageWidth( const uint width );

    void setCollageHeight( const uint height );

    void setFrame( const cl_mem frameImage2D );

    void setFrameWidth( const uint width );

    void setFrameHeight( const uint height );

    void setFrameCenterX( const float centerX );

    void setFrameCenterY( const float centerY );



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

#endif // CLXRayCompositingKernel_H
