#ifndef CLCompositor_H
#define CLCompositor_H

#include <QPixmap>
#include "CLXRayCompositingKernel.h"
#include "CLImage2D.h"
#include "CLImage2DArray.h"
#include "CLBuffer.h"
#include <unordered_map>
#include "CLAbstractCompositor.h"

class CLCompositor : public CLAbstractCompositor
{
    Q_OBJECT
public:

    CLCompositor( const uint64_t gpuIndex ,
                  const uint frameWidth ,
                  const uint frameHeight ) ;

    ~CLCompositor();

    virtual void allocateFrame( CLRenderer *renderer ) override ;

    virtual void collectFrame( CLRenderer *renderer ,
                               const cl_bool block ) override ;

    virtual void composite() override ;

    virtual void loadFinalFrame() override ;

    CLFrame< uint > *&getFinalFrame();

    uint framesCount() const ;

    uint8_t getCompositedFramesCount() const;

protected :

    virtual void initializeBuffers_() override ;

    virtual void initializeKernel_() override ;

protected:
    CLXRayCompositingKernel *compositingKernel_ ;

    const Dimensions2D frameDimensions_;

    uint framesCount_ ;

    uint framesInCompositor_ ;

    CLImage2D< uint > *finalFrame_ ;

    CLImage2D< uint > *finalFrameReadout_ ;

    //empty
    CLImage2DArray< uint > *imagesArray_ ;

    CLBuffer< uint > *depthIndex_ ;
};

#endif // CLCompositor_H
