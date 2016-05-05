#ifndef CLCompositor_H
#define CLCompositor_H

#include <QPixmap>
#include "CLXRayCompositingKernel.h"
#include "CLImage2D.h"
#include "CLImage2DArray.h"
#include "CLBuffer.h"
#include <unordered_map>
#include "CLAbstractCompositor.h"
#include <QMutex>
#include <QMutexLocker>

template< class T >
class CLCompositor : public CLAbstractCompositor
{
public:

    CLCompositor( const uint64_t gpuIndex ,
                  const uint frameWidth , const uint frameHeight,
                  const std::string kernelDirectory = "." ) ;

    ~CLCompositor( );

    void allocateFrame( CLAbstractRenderer *renderer ) override ;

    void collectFrame( CLAbstractRenderer *renderer ,
                       const cl_bool block ) override ;

    void composite( ) override ;

    void loadFinalFrame( ) override ;

    const CLFrameVariant &getFinalFrame( ) const override;

    uint framesCount( ) const ;

    uint8_t getCompositedFramesCount( ) const;

protected :

    void initializeBuffers_( ) override ;

    void initializeKernel_( ) override ;

    void updateKernelsArguments( )  ;


protected:

    uint framesCount_ ;

    uint framesInCompositor_ ;

    CLImage2D< T > *finalFrame_ ;

    CLImage2D< T > *finalFrameReadout_ ;

    //empty
    CLImage2DArray< T > *imagesArray_ ;

    CLBuffer< uint > *depthIndex_ ;

    QMutex criticalMutex_ ;
};

#endif // CLCompositor_H
