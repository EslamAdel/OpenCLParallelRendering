#ifndef CLCOMPOSITORACCUMULATE_H
#define CLCOMPOSITORACCUMULATE_H
#include "CLAbstractCompositor.h"
#include <unordered_map>

#include "CLXRayCompositingKernel.h"

#include <QQueue>


namespace clpar { namespace Compositor {




template< class T >
class CLCompositorAccumulate : public CLAbstractCompositor
{
public:
    CLCompositorAccumulate( const uint64_t gpuIndex,
                            const uint frameWidth ,
                            const uint frameHeight ,
                            const std::string kernelDirectory  );

    void allocateFrame( Renderer::CLAbstractRenderer *renderer ) override ;


    void collectFrame( Renderer::CLAbstractRenderer *renderer ,
                               const cl_bool block ) override ;

    void composite() override ;

    void loadFinalFrame() override ;

    const clData::CLFrameVariant &getFinalFrame( ) const override;

    uint framesCount() const ;

    uint8_t getCompositedFramesCount() const;

    bool readOutReady() const ;

protected :

    virtual void initializeBuffers_() override ;

    virtual void initializeKernel_() override ;

protected:

    const Dimensions2D frameDimensions_;

    uint framesCount_ ;

    uint framesInCompositor_ ;


    uint8_t compositedFramesCount_ ;

    clData::CLFrame< T > *currentFrame_ ;
    clData::CLFrame< T > *finalFrame_ ;

    clData::CLFrame< T > *finalFrameReadout_ ;

    //empty
    std::unordered_map< const Renderer::CLAbstractRenderer * ,
                        clData::CLFrame< T >* > frames_ ;

    QQueue< clData::CLFrame< T >* > loadedFrames_ ;

};


}}

#endif // CLCOMPOSITORACCUMULATE_H
