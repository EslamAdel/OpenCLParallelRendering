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
        Frame
    };

    CLXRayCompositingKernel( const cl_context clContext ,
                             const std::string kernelName = "xray_compositing_accumulate" ,
                             const std::string kernelFile = "xray_compositing.cl" ,
                             const std::string kernelDirectory = "." );


    void setCollegeFrame( const cl_mem collageImage2D );

    void setFrame( const cl_mem frameImage2D );


    void releaseKernel( );

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
