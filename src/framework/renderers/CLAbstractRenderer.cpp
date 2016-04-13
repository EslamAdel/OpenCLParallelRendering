#include "CLAbstractRenderer.h"
#include "Logger.h"

CLAbstractRenderer::CLAbstractRenderer( const uint64_t gpuIndex ,
                                        const uint frameWidth ,
                                        const uint frameHeight ,
                                        const std::string kernelDirectory ,
                                        QObject *parent )
    : gpuIndex_( gpuIndex ) ,
      frameWidth_( frameWidth ) ,
      frameHeight_( frameHeight ) ,
      kernelDirectory_( kernelDirectory ),
      QObject(parent)
{

    /// @note The oclHWDL scans the entire system, and returns a list of
    /// platforms and devices. Since we don't really care about the different
    /// platforms in the system and curious only to get the GPUs, we get a
    /// list of all the GPUs connected to the system in a list and select
    /// one of them based on the given GPU ID.
    initializeContext_( );
}

uint64_t CLAbstractRenderer::getGPUIndex() const
{
    return gpuIndex_ ;
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

bool CLAbstractRenderer::lessThan( const CLAbstractRenderer *lhs ,
                                   const CLAbstractRenderer *rhs )
{
    return lhs->getCurrentCenter().z < rhs->getCurrentCenter().z ;
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

    // Get the platform that corresponds to the GPU
    platform_ = selectedGPU->getPlatform()->getPlatformId();

    // Create the OpenCL context
    oclHWDL::Context* clContext =
            new oclHWDL::Context(selectedGPU, oclHWDL::BASIC_CL_CONTEXT);
    context_ = clContext->getContext();
}

void CLAbstractRenderer::createCommandQueue_()
{
    cl_int clErrorCode;
    commandQueue_ = clCreateCommandQueue( context_,
                                          device_,
                                          0, &clErrorCode );

    oclHWDL::Error::checkCLError(clErrorCode);
}

