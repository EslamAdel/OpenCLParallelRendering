#ifndef CLCOMPOSITORACCUMULATE_H
#define CLCOMPOSITORACCUMULATE_H
#include "CLAbstractCompositor.h"
#include <unordered_map>
#include "CLFrame.h"
#include "CLXRayCompositingKernel.h"

#include <QQueue>


template< class T >
class CLCompositorAccumulate : public CLAbstractCompositor
{
public:
    CLCompositorAccumulate( const uint64_t gpuIndex ,
                            const uint frameWidth ,
                            const uint frameHeight );

    void allocateFrame( CLRenderer *renderer ) override ;


    void collectFrame( CLRenderer *renderer ,
                               const cl_bool block ) override ;

    void composite() override ;

    void loadFinalFrame() override ;

    CLFrameVariant &getFinalFrame( ) override;

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

    CLFrame< T > *currentFrame_ ;
    CLFrame< T > *finalFrame_ ;

    CLFrame< T > *finalFrameReadout_ ;

    //empty
    std::unordered_map< const CLRenderer * ,
                        CLFrame< T >* > frames_ ;

    QQueue< CLFrame< T >* > loadedFrames_ ;

};

#endif // CLCOMPOSITORACCUMULATE_H
