#ifndef CLCOMPOSITINGKERNEL_H
#define CLCOMPOSITINGKERNEL_H

#include "Headers.hh"
#include <oclHWDL/oclHWDL.h>
#include "Logger.h"
#include "CLRenderingKernel.h"
#include <QMap>

class CLCompositingKernel
{
protected:
    enum KernelArgument {
        KERNEL_ARG_FinalFrame = 0 ,
        KERNEL_ARG_Frame ,
        KERNEL_ARG_DepthIndex
    };


public:
    CLCompositingKernel( const cl_context clContext ,
                         const std::string kernelName ,
                         const std::string kernelFile ,
                         const std::string kernelDirectory );

    void setFinalFrame( const cl_mem collageImage2D );

    void setFrame( const cl_mem frameImage2D );

    void setDepthIndex( const cl_mem depthIndex );

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

/**
 * @brief CLRenderingKernels
 */
typedef QMap< RenderingMode , CLCompositingKernel* > CLCompositingKernels;

#endif // CLCOMPOSITINGKERNEL_H
