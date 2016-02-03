#ifndef COMPOSITINGNODE_H
#define COMPOSITINGNODE_H

#include <Headers.hh>
#include <oclHWDL/oclHWDL.h>
#include <QPixmap>
#include "CLFrame.h"
#include "CLXRayCompositingKernel.h"
#include "CLRewindFrameKernel.h"
#include <QObject>

class CompositingNode : public QObject
{
    Q_OBJECT
public:
    CompositingNode( const uint64_t gpuIndex ,
                     const std::vector< Dimensions2D* > framesDimenstions ,
                     const std::vector< Coordinates3D* > framesCenters ,
                     const uint framesCount ,
                     const Dimensions2D collageFrameDimensions );

    ~CompositingNode();

    uint64_t getGPUIndex( ) const;


    uint * &frameData( const uint frameIndex ) ;

    void uploadFrameData( const uint frameIndex );

    void accumulateFrame( const uint frameIndex );

    void rewindCollageFrame();

     QPixmap &getCollagePixmap()  ;

private:
    void selectGPU_( );

    void initializeContext_() ;

    void initializeBuffers_() ;

    void initializeKernel_() ;

    void createCommandQueue_( );

private:

    CLXRayCompositingKernel *compositingKernel_ ;

    CLRewindFrameKernel *rewindFrameKernel_;


    const uint64_t gpuIndex_;

    const std::vector< Dimensions2D* > framesDimensions_ ;

    const std::vector< Coordinates3D* > framesCenters_ ;

    const Dimensions2D collageFrameDimensions_;

    //empty
    std::vector< CLFrame32* > frames_ ;

    //empty
    std::vector< uint* > framesData_ ;

    CLFrame32 *collageFrame_ ;

    const uint framesCount_ ;


    cl_platform_id platform_;


    cl_device_id device_;


    cl_context context_;


    cl_command_queue commandQueue_;



//    oclHWDL::KernelContext* kernelContext_;

};

#endif // COMPOSITINGNODE_H
