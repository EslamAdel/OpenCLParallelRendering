#include "CLXRayRenderingKernel.h"
#include <oclUtils.h>
#include <Logger.h>

#define X_RAY_FRAME_BUFFER_ARG                                              0
#define X_RAY_FRAME_WIDTH_ARG                                               1
#define X_RAY_FRAME_HEIGHT_ARG                                              2
#define X_RAY_VOLUME_DENSITY_ARG                                            3
#define X_RAY_IMAGE_BRIGHTNESS_ARG                                          4
#define X_RAY_INVERSE_MATRIX_ARG                                            5
#define X_RAY_VOLUME_ARRAY_ARG                                              6
#define X_RAY_VOLUME_SAMPLER_ARG                                            7

CLXRayRenderingKernel::CLXRayRenderingKernel( cl_context clContext,
                                              const std::string kernelDirectory,
                                              const std::string kernelFile,
                                              const std::string kernelName )
    : CLRenderingKernel( clContext, kernelDirectory, kernelFile )
    , kernelName_( kernelName )
{
    buildKernel_( );
    retrieveKernelObject_();
}

void CLXRayRenderingKernel::retrieveKernelObject_( )
{
    kernelObject_ = kernelContext_->getKernelObject( kernelName_ );
}

RENDERING_KERNEL_TYPE CLXRayRenderingKernel::getRenderingKernelType( ) const
{
    return X_RAY;
}

void CLXRayRenderingKernel::setFrameBuffer( cl_mem frameBuffer )
{
    cl_int error = clSetKernelArg( kernelObject_, X_RAY_FRAME_BUFFER_ARG,
                                   sizeof( cl_mem ),
                                   ( void* ) &frameBuffer );
     oclCheckErrorEX( error, CL_SUCCESS, 0 );
}

void CLXRayRenderingKernel::setFrameWidth( uint width )
{
    cl_int error = clSetKernelArg( kernelObject_, X_RAY_FRAME_WIDTH_ARG,
                                   sizeof( uint ),
                                   &width );
    oclCheckErrorEX( error, CL_SUCCESS, 0 );
}

void CLXRayRenderingKernel::setFrameHeight( uint height )
{
    cl_int error = clSetKernelArg( kernelObject_, X_RAY_FRAME_HEIGHT_ARG,
                                   sizeof( uint ),
                                   &height );
    oclCheckErrorEX( error, CL_SUCCESS, 0 );
}

void CLXRayRenderingKernel::setVolumeData( cl_mem data )
{
    cl_int error = clSetKernelArg( kernelObject_, X_RAY_VOLUME_ARRAY_ARG,
                                   sizeof( cl_mem ),
                                   ( void* ) &data );
     oclCheckErrorEX( error, CL_SUCCESS, 0 );
}

void CLXRayRenderingKernel::setVolumeSampler( cl_sampler sampler )
{
    cl_int error = clSetKernelArg( kernelObject_, X_RAY_VOLUME_SAMPLER_ARG,
                                   sizeof( cl_sampler ),
                                   &sampler );
    oclCheckErrorEX( error, CL_SUCCESS, 0 );
}

void CLXRayRenderingKernel::setImageBrightnessFactor( float brightness )
{
    cl_int error = clSetKernelArg( kernelObject_, X_RAY_IMAGE_BRIGHTNESS_ARG,
                                   sizeof( float ),
                                   &brightness );
    oclCheckErrorEX( error, CL_SUCCESS, 0 );
}

void CLXRayRenderingKernel::setVolumeDensityFactor( float density )
{
    cl_int error = clSetKernelArg( kernelObject_, X_RAY_VOLUME_DENSITY_ARG,
                                   sizeof( float ),
                                   &density );
    oclCheckErrorEX( error, CL_SUCCESS, 0 );
}

void CLXRayRenderingKernel::setInverseViewMatrix( cl_mem matrix )
{
    cl_int error = clSetKernelArg( kernelObject_, X_RAY_INVERSE_MATRIX_ARG,
                                   sizeof( cl_mem ),
                                   ( void* ) &matrix );
    oclCheckErrorEX( error, CL_SUCCESS, 0 );
}
