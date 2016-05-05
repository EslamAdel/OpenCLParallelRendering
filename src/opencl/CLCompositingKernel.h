#ifndef CLCOMPOSITINGKERNEL_H
#define CLCOMPOSITINGKERNEL_H

#include "Headers.hh"
#include <oclHWDL/oclHWDL.h>
#include "Logger.h"
#include "CLRenderingKernel.h"
#include <QMap>

class CLCompositingKernel
{

    Q_DISABLE_COPY( CLCompositingKernel )

protected:
    enum KernelArgument {
        KERNEL_ARG_FinalFrame = 0 ,
        KERNEL_ARG_Frame ,
        KERNEL_ARG_DepthIndex
    };


public:
    /**
     * @brief CLCompositingKernel
     * @param clContext
     * @param kernelName
     * @param kernelFile
     * @param kernelDirectory
     */
    CLCompositingKernel( const cl_context clContext ,
                         const std::string kernelName ,
                         const std::string kernelFile ,
                         const std::string kernelDirectory );

    /**
     * @brief setFinalFrame
     * @param collageImage2D
     */
    void setFinalFrame( const cl_mem collageImage2D );

    /**
     * @brief setFrame
     * @param frameImage2D
     */
    void setFrame( const cl_mem frameImage2D );

    /**
     * @brief setDepthIndex
     * @param depthIndex
     */
    void setDepthIndex( const cl_mem depthIndex );

    /**
     * @brief releaseKernel
     */
    void releaseKernel( );

    /**
     * @brief getKernelObject
     * @return
     */
    cl_kernel getKernelObject( ) const;

private:

    /**
     * @brief buildKernel_
     * @param extraBuildOptions
     */
    void buildKernel_( const std::string extraBuildOptions = "" );

    /**
     * @brief retrieveKernelObject_
     */
    void retrieveKernelObject_( );

private:
    /**
     * @brief clContext_
     */
    cl_context clContext_;

    /**
     * @brief kernelContext_
     */
    oclHWDL::KernelContext* kernelContext_;

    /**
     * @brief kernelObject_
     */
    cl_kernel kernelObject_;

    /**
     * @brief kernelName_
     */
    const std::string kernelName_;

    /**
     * @brief kernelDirectory_
     */
    const std::string kernelDirectory_;

    /**
     * @brief kernelFile_
     */
    const std::string kernelFile_;
};

/**
 * @brief CLRenderingKernels
 */
typedef QMap< RenderingMode , CLCompositingKernel* > CLCompositingKernels;

#endif // CLCOMPOSITINGKERNEL_H
