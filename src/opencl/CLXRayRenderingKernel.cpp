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
#define X_RAY_TRANSFER_FUNCTION_OFFSET_ARG                                  8
#define X_RAY_TRANSFER_FUNCTION_SCALE_ARG                                   9
#define X_RAY_TRANSFER_FUNCTION_DATA_ARG                                    10
#define X_RAY_TRANSFER_FUNCTION_SAMPLER_ARG                                 11
#define X_RAY_TRANSFER_FUNCTION_FLAG_ARG                                    12

CLXRayRenderingKernel::CLXRayRenderingKernel(cl_context clContext,
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

    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }

}

void CLXRayRenderingKernel::setFrameWidth( uint width )
{
    cl_int error = clSetKernelArg( kernelObject_, X_RAY_FRAME_WIDTH_ARG,
                                   sizeof( uint ),
                                   &width );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}

void CLXRayRenderingKernel::setFrameHeight( uint height )
{
    cl_int error = clSetKernelArg( kernelObject_, X_RAY_FRAME_HEIGHT_ARG,
                                   sizeof( uint ),
                                   &height );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}

void CLXRayRenderingKernel::setVolumeData( cl_mem data )
{
    cl_int error = clSetKernelArg( kernelObject_, X_RAY_VOLUME_ARRAY_ARG,
                                   sizeof( cl_mem ),
                                   ( void* ) &data );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}

void CLXRayRenderingKernel::setVolumeSampler( cl_sampler sampler )
{
    cl_int error = clSetKernelArg( kernelObject_, X_RAY_VOLUME_SAMPLER_ARG,
                                   sizeof( cl_sampler ),
                                   &sampler );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}

void CLXRayRenderingKernel::setImageBrightnessFactor( float brightness )
{
    cl_int error = clSetKernelArg( kernelObject_, X_RAY_IMAGE_BRIGHTNESS_ARG,
                                   sizeof( float ),
                                   &brightness );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}

void CLXRayRenderingKernel::setVolumeDensityFactor( float density )
{
    cl_int error = clSetKernelArg( kernelObject_, X_RAY_VOLUME_DENSITY_ARG,
                                   sizeof( float ),
                                   &density );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}

void CLXRayRenderingKernel::setInverseViewMatrix( cl_mem matrix )
{
    cl_int error = clSetKernelArg( kernelObject_, X_RAY_INVERSE_MATRIX_ARG,
                                   sizeof( cl_mem ),
                                   ( void* ) &matrix );
    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("Exiting Due to OpenCL Error!");
    }
}

//void CLXRayRenderingKernel::setTransferFunctionData(cl_mem data)
//{
//    cl_int error = clSetKernelArg(kernelObject_,X_RAY_TRANSFER_FUNCTION_DATA_ARG,
//                                  sizeof( cl_mem ),
//                                  (void *) &data);
//    if( error != CL_SUCCESS )
//    {
//        oclHWDL::Error::checkCLError( error );
//        LOG_ERROR("Exiting Due to OpenCL Error!");
//    }
//}

//void CLXRayRenderingKernel::setTransferFunctionOffset(float offset)
//{
//    cl_int error = clSetKernelArg(kernelObject_,X_RAY_TRANSFER_FUNCTION_OFFSET_ARG,
//                                  sizeof( float ),
//                                  (void* ) &offset);
//    if( error != CL_SUCCESS )
//    {
//        oclHWDL::Error::checkCLError( error );
//        LOG_ERROR("Exiting Due to OpenCL Error!");
//    }
//}

//void CLXRayRenderingKernel::setTransferFunctionSampler(cl_sampler sampler)
//{
//    cl_int error = clSetKernelArg( kernelObject_, X_RAY_TRANSFER_FUNCTION_SAMPLER_ARG,
//                                   sizeof( cl_sampler ),
//                                   &sampler );
//    if( error != CL_SUCCESS )
//    {
//        oclHWDL::Error::checkCLError( error );
//        LOG_ERROR("Exiting Due to OpenCL Error!");
//    }
//}

//void CLXRayRenderingKernel::setTransferFunctionScale(float scale)
//{
//    cl_int error = clSetKernelArg(kernelObject_,X_RAY_TRANSFER_FUNCTION_SCALE_ARG,
//                                  sizeof( float ),
//                                  (void *)&scale);
//    if( error != CL_SUCCESS )
//    {
//        oclHWDL::Error::checkCLError( error );
//        LOG_ERROR("Exiting Due to OpenCL Error!");
//    }
//}

//void CLXRayRenderingKernel::setTransferFunctionFlag(int enableTransferFunction)
//{
//    cl_int error = clSetKernelArg( kernelObject_, X_RAY_TRANSFER_FUNCTION_FLAG_ARG,
//                                   sizeof( int ),
//                                   &enableTransferFunction );
//    if( error != CL_SUCCESS )
//    {
//        oclHWDL::Error::checkCLError( error );
//        LOG_ERROR("Exiting Due to OpenCL Error!");
//    }
//}
