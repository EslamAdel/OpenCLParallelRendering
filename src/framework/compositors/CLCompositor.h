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

    /**
     * @brief CLCompositor
     * @param gpuIndex
     * @param frameWidth
     * @param frameHeight
     * @param kernelDirectory
     */
    CLCompositor( const uint64_t gpuIndex ,
                  const uint frameWidth , const uint frameHeight,
                  const std::string kernelDirectory = "." ) ;

    ~CLCompositor( );

    /**
     * @brief allocateFrame
     * @param renderer
     */
    void allocateFrame( CLAbstractRenderer *renderer ) override ;

    /**
     * @brief collectFrame
     * @param renderer
     * @param block
     */
    void collectFrame( CLAbstractRenderer *renderer ,
                       const cl_bool block ) override ;

    /**
     * @brief composite
     */
    void composite( ) override ;

    /**
     * @brief loadFinalFrame
     */
    void loadFinalFrame( ) override ;

    /**
     * @brief getFinalFrame
     * @return
     */
    const CLFrameVariant &getFinalFrame( ) const override;

    /**
     * @brief framesCount
     * @return
     */
    uint framesCount( ) const ;

    /**
     * @brief getCompositedFramesCount
     * @return
     */
    uint8_t getCompositedFramesCount( ) const;

protected :

    /**
     * @brief initializeBuffers_
     */
    void initializeBuffers_( ) override ;

    /**
     * @brief initializeKernel_
     */
    void initializeKernel_( ) override ;

    /**
     * @brief updateKernelsArguments
     */
    void updateKernelsArguments( )  ;


protected:

    /**
     * @brief framesCount_
     */
    uint framesCount_ ;

    /**
     * @brief framesInCompositor_
     */
    uint framesInCompositor_ ;

    /**
     * @brief finalFrame_
     */
    CLImage2D< T > *finalFrame_ ;

    /**
     * @brief finalFrameReadout_
     */
    CLImage2D< T > *finalFrameReadout_ ;



    //empty
    /**
     * @brief imagesArray_
     */
    CLImage2DArray< T > *imagesArray_ ;

    /**
     * @brief depthIndex_
     */
    CLBuffer< uint > *depthIndex_ ;

    /**
     * @brief criticalMutex_
     */
    QMutex criticalMutex_ ;
};

#endif // CLCompositor_H
