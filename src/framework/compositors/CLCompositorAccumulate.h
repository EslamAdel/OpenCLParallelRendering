#ifndef CLCOMPOSITORACCUMULATE_H
#define CLCOMPOSITORACCUMULATE_H
#include "CLAbstractCompositor.h"
#include <unordered_map>

#include "CLXRayCompositingKernel.h"

#include <QQueue>


namespace clparen { namespace Compositor {




template< class T >
class CLCompositorAccumulate : public CLAbstractCompositor
{
public:
    CLCompositorAccumulate( const uint64_t gpuIndex,
                            const uint frameWidth ,
                            const uint frameHeight ,
                            const std::string kernelDirectory  );

    void allocateFrame( Renderer::CLAbstractRenderer *renderer ) Q_DECL_OVERRIDE ;


    void collectFrame( Renderer::CLAbstractRenderer *renderer ,
                               const cl_bool block ) Q_DECL_OVERRIDE ;

    void composite() Q_DECL_OVERRIDE ;

    void loadFinalFrame() Q_DECL_OVERRIDE ;

    const CLData::CLFrameVariant &getFinalFrame( ) const Q_DECL_OVERRIDE;

    uint framesCount() const ;

    uint8_t getCompositedFramesCount() const;

    bool readOutReady() const ;

    bool isRenderingModeSupported( CLKernel::RenderingMode mode ) Q_DECL_OVERRIDE;

protected :

    virtual void initializeBuffers_() Q_DECL_OVERRIDE ;

    virtual void initializeKernel_() Q_DECL_OVERRIDE ;

protected:

    const Dimensions2D frameDimensions_;

    uint framesCount_ ;

    uint framesInCompositor_ ;


    uint8_t compositedFramesCount_ ;

    CLData::CLFrame< T > *currentFrame_ ;
    CLData::CLFrame< T > *finalFrame_ ;

    CLData::CLFrame< T > *finalFrameReadout_ ;

    //empty
    std::unordered_map< const Renderer::CLAbstractRenderer * ,
                        CLData::CLFrame< T >* > frames_ ;

    QQueue< CLData::CLFrame< T >* > loadedFrames_ ;

};


}}

#endif // CLCOMPOSITORACCUMULATE_H
