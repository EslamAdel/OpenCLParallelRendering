#ifndef COMPOSITINGNODE_H
#define COMPOSITINGNODE_H

#include <Headers.hh>
#include <oclHWDL/oclHWDL.h>
#include <QPixmap>
#include "CLFrame.h"
#include "CLXRayCompositingKernel.h"
#include "CLRewindFrameKernel.h"
#include <QObject>
#include "CLImage2D.h"
#include "CLImage2DArray.h"
#include <unordered_map>
#include "RenderingNode.h"

typedef std::unordered_map< const RenderingNode * , CLFrame32* > Frames ;

/**
 * @brief The CompositingNode class
 * -Wrapping the OpenCL Context where compositing performed.
 * -Managing the compositing operations.
 * -Wrapping two kernel objects, a kernel for compositor and
 * a kernel for rewinding (i.e clear collage buffer).
 *
 * The proper use of the compositing node:
 * When rendering node has properly rendered a frame, and then the
 * buffer is uploaded to host with pointer ptr.
 *
 * 1| CompositingNode::setFrameData_HOST( frameIndex , ptr );
 * 2| CompositingNode::loadFrameDataToDevice( frameIndex, block);
 * 3| CompositingNode::accumulateFrame_DEVICE( frameIndex );
 * 4| Repeat 1-3 until for each compositing frame.
 * 5| CompositingNode::uploadCollageFromDevice();
 * 6| CompositingNode::getCollagePixmap();
 * 7| CompositingNode::rewindCollageFrame_DEVICE( blocking );
 * 8| repeat 1-7 for each rendering loop.
 *
 */
class CompositingNode : public QObject
{
    Q_OBJECT
public:

    /**
     * @brief CompositingNode
     * @param gpuIndex
     * The worker device.
     * @param framesCount
     * Number of frames to be composited.
     * @param frameWidth
     * @param frameHeight
     * The collage frame and the compositing frames,
     * should be with the same width and height. Otherwise, the behaviour
     * of OpenCL kernel execution is not predicted.
     */
    CompositingNode( const uint64_t gpuIndex ,
                     const uint frameWidth ,
                     const uint frameHeight ) ;

    ~CompositingNode();



    virtual void allocateFrame( RenderingNode *renderingNode );

    virtual void collectFrame( RenderingNode *renderingNode ,
                               const cl_bool block );
    /**
     * @brief accumulateFrame_DEVICE
     * This method initiates the operation of accumulating the compositing
     * frame to the collage frame, must not be invoked before the compositing
     * buffer is completely loaded to device and no other operation is performed
     * on the collage buffer.
     *
     * @param frameIndex
     * the index of the corresponding compositing frame.
     */
    void accumulateFrame_DEVICE( RenderingNode *renderingNode );


    /**
     * @brief uploadCollageFromDevice
     * Called after all compositing buffers accumulated to collage buffer.
     */
    void loadCollageFromDevice( )  ;


    /**
     * @brief getGPUIndex
     * @return
     * return the index of the worker device.
     */
    uint64_t getGPUIndex( ) const;

    /**
     * @brief getCollagePixmap
     * This method is called after the compositing of all frames.
     * @return
     * return the collage Pixmap.
     */
    CLFrame32 *&getCLFrameCollage();


    /**
     * @brief framesCount
     * @return
     */
    uint framesCount() const ;

    uint8_t getCompositedFramesCount() const;



private:
    /**
     * @brief selectGPU_
     */
    void selectGPU_( );

    /**
     * @brief initializeContext_
     */
    void initializeContext_() ;

protected:
    /**
     * @brief initializeBuffers_
     * Allocate framesCount_ buffers on the compositng device, in addition
     * to the collage buffer.
     */
    void initializeBuffers_() ;

    /**
     * @brief initializeKernel_
     * Constructs the compositing kernel and the rewinding kernel, also
     * the collage buffer is passed to the rewinding kernel, as it will
     * always be rewinded after compositing is completely done.
     */
    void initializeKernel_() ;

private:
    /**
     * @brief createCommandQueue_
     */
    void createCommandQueue_( );

protected:


    //Kernel wrapped objects.
    /**
     * @brief compositingKernel_
     */
    CLXRayCompositingKernel *compositingKernel_ ;


    //facts
    const uint64_t gpuIndex_;

    const Dimensions2D collageFrameDimensions_;

    uint framesCount_ ;

    uint8_t compositedFramesCount_ ;

    cl_platform_id platform_;

    cl_device_id device_;

    cl_context context_;

    cl_command_queue commandQueue_;


    /**
     * @brief collageFrame_
     * Collage frame object.
     */
    CLFrame32 *collageFrame_ ;

    CLFrame32 *collageFrameReadout_ ;

    //empty
    Frames frames_ ;

};

#endif // COMPOSITINGNODE_H
