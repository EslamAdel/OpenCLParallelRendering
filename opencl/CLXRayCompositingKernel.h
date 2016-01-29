#ifndef CLXRayCompositingKernel_H
#define CLXRayCompositingKernel_H


#include "Headers.hh"
#include <oclHWDL/oclHWDL.h>
#include <QVector>
#include "CLImage.h"

class CLXRayCompositingKernel
{
public:

    /**
     * @brief The Mode enum
     * Mode::Batch :
     * Compositing will be done in all images at once.
     *
     * Mode::Accumulator :
     * Compositing will be done at one Image at a time.
     */

    enum Mode
    {
        Batch ,
        Accumulator
    };

    enum AccumulatorModeArgs
    {
        BaseBuffer = 0 ,
        StackedBuffer
    };

    CLXRayCompositingKernel( const uint frameWidth ,
                             const uint frameHeight,
                             const uint elementFramesCount,
                             const Mode compositingMode = Mode::Accumulator ,
                             const cl_context clContext = NULL,
                             const std::string kernelDirectory = ".",
                             const std::string kernelFile = "xray-compositing.cl" ,
                             const std::string kernelName = "xray-compositing");



    void accumulateBuffer(const cl_mem baseBuffer,
                          const cl_mem buffer ,
                          cl_command_queue cmdQueue);




    void releaseKernel( );


    std::string getKernelDirectory( ) const;


    std::string getKernelFile( ) const;

    oclHWDL::KernelContext *getKernelContext( ) const;

    cl_kernel getKernelObject( ) const;


private:

    void buildKernel_( const std::string extraBuildOptions = "" );

    void retrieveKernelObject_( );

    cl_context clContext_;
    oclHWDL::KernelContext* kernelContext_;
    cl_kernel kernelObject_;


    // facts

    const std::string kernelName_;\
    const std::string kernelDirectory_;
    const std::string kernelFile_;

    const Mode compositingMode_;

    const uint frameWidth_ ;
    const uint frameHeight_ ;
    const uint8_t framesCount_ ;


};

#endif // CLXRayCompositingKernel_H
