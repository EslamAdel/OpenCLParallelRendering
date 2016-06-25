#include "CLAbstractRenderer.h"
#include "Logger.h"
#include <system/Utilities.h>

namespace clparen {
namespace Renderer {


CLAbstractRenderer::CLAbstractRenderer(
        const uint64_t gpuIndex,
        const Dimensions2D frameDimensions,
        const CLData::FRAME_CHANNEL_ORDER frameChannelOrder,
        const std::string kernelDirectory,
        QObject *parent )
    : gpuIndex_( gpuIndex ) ,
      frameDimensions_( frameDimensions ) ,
      sortFirstOffset_( Dimensions2D( 0 , 0 )) ,
      sortFirstDimensions_( frameDimensions ) ,
      kernelDirectory_( kernelDirectory ),
      frameChannelOrder_( frameChannelOrder ),
      QObject( parent )
{
    renderingTime_ = 1.f ;
    /// @note The oclHWDL scans the entire system, and returns a list of
    /// platforms and devices. Since we don't really care about the different
    /// platforms in the system and curious only to get the GPUs, we get a
    /// list of all the GPUs connected to the system in a list and select
    /// one of them based on the given GPU ID.
    initializeContext_( );
    renderingKernels_ = allocateKernels_();


    activeRenderingKernel_ =
            renderingKernels_[ defaultRenderingMode_() ];

}


uint64_t CLAbstractRenderer::getGPUIndex() const
{
    return gpuIndex_ ;
}

std::string CLAbstractRenderer::getGPUName() const
{
    return name_ ;
}

cl_platform_id CLAbstractRenderer::getPlatformId() const
{
    return platform_ ;
}

cl_device_id CLAbstractRenderer::getDeviceId() const
{
    return device_ ;
}

cl_context CLAbstractRenderer::getContext() const
{
    return context_ ;
}

cl_command_queue CLAbstractRenderer::getCommandQueue() const
{
    return commandQueue_ ;
}

void CLAbstractRenderer::switchRenderingKernel( const CLKernel::RenderingMode mode )
{

    QMutexLocker lock( &switchKernelMutex_ );

    if( !renderingKernels_.contains( mode ))
        return;

    if( !isRenderingModeSupported( mode ))
    {
        LOG_WARNING("Rendering mode is not supported by renderer");
        return;
    }

    activeRenderingMode_ = mode ;

    activeRenderingKernel_ = renderingKernels_[ mode ];

}

bool CLAbstractRenderer::lessThan( const CLAbstractRenderer *lhs ,
                                   const CLAbstractRenderer *rhs )
{
    return lhs->getCurrentCenter().z < rhs->getCurrentCenter().z ;
}

double CLAbstractRenderer::getRenderingTime() const
{
    return renderingTime_;
}

CLKernel::RenderingMode CLAbstractRenderer::defaultRenderingMode_() const
{
    if( frameChannelOrder_ == CLData::FRAME_CHANNEL_ORDER::ORDER_INTENSITY )
        return CLKernel::RenderingMode::RENDERING_MODE_Xray ;
    else
        return CLKernel::RenderingMode::RENDERING_MODE_Ultrasound;
}

void CLAbstractRenderer::initializeContext_()
{
    LOG_DEBUG( "Initializing an OpenCL context ... " );

    selectGPU_();
    createCommandQueue_();

    LOG_DEBUG( "[DONE] Initializing an OpenCL context ... " );
}

void CLAbstractRenderer::selectGPU_()
{
    // Scan the hardware
    oclHWDL::Hardware* clHardware = new oclHWDL::Hardware();

    // Get a list of all the GPUs that are connected to the system
    const oclHWDL::Devices listGPUs = clHardware->getListGPUs();

    // Select the GPU that will be used for running the kernel
    oclHWDL::Device* selectedGPU = listGPUs.at( gpuIndex_ );
    device_ = selectedGPU->getId();
    name_ = selectedGPU->getName();

    // Get the platform that corresponds to the GPU
    platform_ = selectedGPU->getPlatform()->getPlatformId();

    // Create the OpenCL context
    oclHWDL::Context* clContext =
            new oclHWDL::Context( selectedGPU, oclHWDL::BASIC_CL_CONTEXT );
    context_ = clContext->getContext();
}

void CLAbstractRenderer::createCommandQueue_()
{
    cl_int clErrorCode;
    commandQueue_ = clCreateCommandQueue( context_,
                                          device_,
                                          CL_QUEUE_PROFILING_ENABLE,
                                          &clErrorCode );

    oclHWDL::Error::checkCLError(clErrorCode);
}


CLKernel::CLRenderingKernels
CLAbstractRenderer::allocateKernels_() const
{

    CLKernel::CLRenderingKernels kernels;

    /// Add all the rendering kernel here, and set the selected to be the
    /// activeRenderingKernel_
    kernels[ CLKernel::RenderingMode::RENDERING_MODE_Xray ] =
            new CLKernel::CLXRayRenderingKernel(
                context_ ,
                kernelDirectory_ );

    kernels[ CLKernel::RenderingMode::RENDERING_MODE_MaxIntensity ] =
            new CLKernel::CLMaxIntensityProjectionRenderingKernel(
                context_ ,
                kernelDirectory_ );

    kernels[ CLKernel::RenderingMode::RENDERING_MODE_MinIntensity ] =
            new CLKernel::CLMinIntensityProjectionRenderingKernel(
                context_ ,
                kernelDirectory_ );


    kernels[ CLKernel::RenderingMode::RENDERING_MODE_AverageIntensity ] =
            new CLKernel::CLAverageIntensityProjectionRenderingKernel(
                context_ ,
                kernelDirectory_ );

    kernels[ CLKernel::RenderingMode::RENDERING_MODE_IsoSurface ] =
            new CLKernel::CLIsoSurfaceRenderingKernel(
                context_ ,
                kernelDirectory_ );

    kernels[ CLKernel::RenderingMode::RENDERING_MODE_Shaded ] =
            new CLKernel::CLShadedRenderingKernel(
                context_ ,
                kernelDirectory_ );

    kernels[ CLKernel::RenderingMode::RENDERING_MODE_Ultrasound ] =
            new CLKernel::CL3DUltrasoundRenderingKernel(
                context_ ,
                kernelDirectory_ );

    return kernels ;
}

const Dimensions2D &CLAbstractRenderer::getSortFirstDimensions() const
{
    return sortFirstDimensions_;
}

void CLAbstractRenderer::setSortFirstSettings(
        const Dimensions2D sortFirstOffset,
        const Dimensions2D sortFirstDimensions )
{
    sortFirstDimensions_ = sortFirstDimensions ;
    sortFirstOffset_ = sortFirstOffset ;

    if( sortFirstOffset_.x + sortFirstDimensions_.x > frameDimensions_.x )
        LOG_ERROR("Rendered region exceeds the frame region. (%ld>%ld)" ,
                  sortFirstOffset_.x + sortFirstDimensions_.x ,
                  frameDimensions_.x );

    if( sortFirstOffset_.y + sortFirstDimensions_.y > frameDimensions_.y )
        LOG_ERROR("Rendered region exceeds the frame region. (%ld>%ld)" ,
                  sortFirstOffset_.y + sortFirstDimensions_.y ,
                  frameDimensions_.y );


    setRegion_();

}


float CLAbstractRenderer::calculateRenderingTime_()
{
    // Assuming that every thing is going in the right direction.
    cl_int clErrorCode = CL_SUCCESS;

    cl_ulong start , end;



    clErrorCode |=
            clGetEventProfilingInfo( clGPUExecution_,
                                     CL_PROFILING_COMMAND_END,
                                     sizeof(cl_ulong),
                                     &end,
                                     0 );

    clErrorCode |=
            clGetEventProfilingInfo( clGPUExecution_,
                                     CL_PROFILING_COMMAND_START,
                                     sizeof(cl_ulong),
                                     &start,
                                     0 );


    if( clErrorCode != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( clErrorCode );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }

    return static_cast< float >( end -  start ) / 1e6 ;
}

const Dimensions2D &CLAbstractRenderer::getSortFirstOffset() const
{
    return sortFirstOffset_;
}

const Dimensions2D &CLAbstractRenderer::getFrameDimensions() const
{
    return frameDimensions_;
}



}
}
