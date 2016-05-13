#include "CLAbstractCompositor.h"

namespace clparen {
namespace Compositor {


CLAbstractCompositor::CLAbstractCompositor( const uint64_t gpuIndex,
                                            const uint frameWidth,
                                            const uint frameHeight,
                                            const std::string kernelDirectory,
                                            QObject *parent)
    : gpuIndex_( gpuIndex ) ,
      kernelDirectory_( kernelDirectory ),
      frameDimensions_( frameWidth , frameHeight ),
      QObject(parent)
{
    readOutReady_ = false ;
    initializeContext_( );
    compositingKernels_ = allocateKernels_();
}

uint64_t CLAbstractCompositor::getGPUIndex() const
{
    return gpuIndex_ ;
}

bool CLAbstractCompositor::readOutReady() const
{
    return readOutReady_ ;
}

void CLAbstractCompositor::switchCompositingKernel(
        const clKernel::RenderingMode mode )
{
    QMutexLocker lock( &switchKernelMutex_ );

    if( compositingKernels_.contains( mode ))

        activeCompositingKernel_ = compositingKernels_[ mode ];
}

void CLAbstractCompositor::selectGPU_()
{
    // Scan the hardware
    oclHWDL::Hardware* clHardware = new oclHWDL::Hardware( );

    // Get a list of all the GPUs that are connected to the system
    const oclHWDL::Devices listGPUs = clHardware->getListGPUs( );

    // Select the GPU that will be used for running the kernel
    oclHWDL::Device* selectedGPU = listGPUs.at( gpuIndex_ );
    device_ = selectedGPU->getId();

    // Get the platform that corresponds to the GPU
    platform_ = selectedGPU->getPlatform()->getPlatformId();

    // Create the OpenCL context
    oclHWDL::Context* clContext =
            new oclHWDL::Context(selectedGPU, oclHWDL::BASIC_CL_CONTEXT );
    context_ = clContext->getContext( );
}

void CLAbstractCompositor::initializeContext_()
{
    LOG_DEBUG( "Initializing an OpenCL context ... " );

    selectGPU_( );
    createCommandQueue_( );

    LOG_DEBUG( "[DONE] Initializing an OpenCL context ... " );
}


clKernel::CLCompositingKernels
CLAbstractCompositor::allocateKernels_( ) const
{

    clKernel::CLCompositingKernels kernels ;


    kernels[ clKernel::RenderingMode::RENDERING_MODE_Xray ] =
            new clKernel::CLXRayCompositingKernel( context_ ,
                                         "xray_compositing_patch" ,
                                         "xray_compositing.cl" ,
                                         kernelDirectory_ );


    kernels[ clKernel::RenderingMode::RENDERING_MODE_MinIntensity ] =
            new clKernel::CLMinIntensityProjectionCompositingKernel(
                context_ ,
                "minIntensityProjection_compositing" ,
                "minIntensityProjection_compositing.cl",
                kernelDirectory_ );


    kernels[ clKernel::RenderingMode::RENDERING_MODE_MaxIntensity ] =
            new clKernel::CLMaxIntensityProjectionCompositingKernel(
                context_ ,
                "maxIntensityProjection_compositing" ,
                "maxIntensityProjection_compositing.cl" ,
                kernelDirectory_ );


    return kernels;
}

void CLAbstractCompositor::createCommandQueue_()
{
    cl_int clErrorCode = CL_SUCCESS;
    commandQueue_ = clCreateCommandQueue( context_ ,
                                          device_ ,
                                          0, &clErrorCode );

    if( clErrorCode != CL_SUCCESS )
        oclHWDL::Error::checkCLError( clErrorCode );
}



}
}
