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
                     const uint framesCount ,
                     const uint frameWidth,
                     const uint frameHeight ) ;

    ~CompositingNode();

    uint64_t getGPUIndex( ) const;


    void loadFrameDataToDevice(const uint frameIndex , const cl_bool block);

    void accumulateFrame_DEVICE( const uint frameIndex );

    void rewindCollageFrame_DEVICE( cl_bool blocking );

    void uploadCollageFromDevice( )  ;

    void setFrameData_HOST(const uint frameIndex, uint *data);

    QPixmap &getCollagePixmap();

    uint framesCount() const ;

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




};

#endif // COMPOSITINGNODE_H
