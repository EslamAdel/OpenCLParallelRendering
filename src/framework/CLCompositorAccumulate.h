#ifndef CLCOMPOSITORACCUMULATE_H
#define CLCOMPOSITORACCUMULATE_H
#include "CLAbstractCompositor.h"
#include <unordered_map>
#include "CLFrame.h"
#include "CLXRayCompositingKernel.h"

#include <QQueue>

typedef std::unordered_map< const CLRenderer * , CLFrame32* > Frames ;


class CLCompositorAccumulate : public CLAbstractCompositor
{
public:
    CLCompositorAccumulate( const uint64_t gpuIndex ,
                            const uint frameWidth ,
                            const uint frameHeight );

    virtual void allocateFrame( CLRenderer *renderer ) override ;


    virtual void collectFrame( CLRenderer *renderer ,
                               const cl_bool block ) override ;

    virtual void composite() override ;

    virtual void loadFinalFrame() override ;

    CLFrame< uint > *&getFinalFrame();

    uint framesCount() const ;

    uint8_t getCompositedFramesCount() const;

    bool readOutReady() const ;

protected :

    virtual void initializeBuffers_() override ;

    virtual void initializeKernel_() override ;

protected:
    CLXRayCompositingKernel *compositingKernel_ ;

    const Dimensions2D frameDimensions_;

    uint framesCount_ ;

    uint framesInCompositor_ ;


    uint8_t compositedFramesCount_ ;

    CLFrame< uint > *currentFrame_ ;
    CLFrame< uint > *finalFrame_ ;

    CLFrame< uint > *finalFrameReadout_ ;

    //empty
    Frames frames_ ;
    QQueue< CLFrame< uint >* > loadedFrames_ ;


};

#endif // CLCOMPOSITORACCUMULATE_H
