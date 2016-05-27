#include "CLTransferFunction.h"
#include "Logger.h"

namespace clparen {
namespace CLData {


CLTransferFunction::CLTransferFunction(
        const uint length ,
        const float *transferFunctionData )
    : length_( length ) ,
      CLImage2D< float >( Dimensions2D( length , 4 ) , CL_RGBA , CL_FLOAT )
{
    this->copyHostData( transferFunctionData );

    this->imageDescriptor_.image_height = 1 ;
}

void CLTransferFunction::createDeviceData( cl_context context )
{
    const cl_mem_flags flags = CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY ;

    this->CLImage2D< float >::createDeviceData( context , flags );
}


void CLTransferFunction::writeDeviceData(
        cl_command_queue cmdQueue ,
        const cl_bool blocking )
{
    const size_t origin[3] = { 0 , 0 , 0 };
    const size_t region[3] = { length_ , 1 , 1 };

    // Initially, assume that everything is fine
    cl_int error = CL_SUCCESS;
    error = clEnqueueWriteImage(
                cmdQueue, this->deviceData_ , blocking ,
                origin , region ,
                length_ * sizeof( float )  ,
                0 ,
                ( const void * ) this->hostData_ ,
                0 , 0 , 0 );

    if( error != CL_SUCCESS )
    {
        oclHWDL::Error::checkCLError( error );
        LOG_ERROR("OpenCL Error!");
    }
}


}
}
