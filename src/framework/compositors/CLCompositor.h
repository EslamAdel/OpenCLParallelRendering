#ifndef CLCompositor_H
#define CLCompositor_H

#include <QPixmap>
#include "CLXRayCompositingKernel.h"
#include "CLAbstractCompositor.h"
#include <QMutex>
#include <QMutexLocker>

namespace clparen {
namespace Compositor {

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
    CLCompositor(
            const uint64_t gpuIndex ,
            const uint frameWidth , const uint frameHeight,
            const std::string kernelDirectory = DEFAULT_KERNELS_DIRECTORY ) ;

    ~CLCompositor( );

    /**
     * @brief allocateFrame
     * @param renderer
     */
    void allocateFrame( Renderer::CLAbstractRenderer *renderer ) override ;

    /**
     * @brief collectFrame
     * @param renderer
     * @param block
     */
    void collectFrame( Renderer::CLAbstractRenderer *renderer ,
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
    const clData::CLFrameVariant &getFinalFrame( ) const override;

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
    void updateKernelsArguments_( )  ;


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
    clData::CLImage2D< T > *finalFrame_ ;

    /**
     * @brief finalFrameReadout_
     */
    clData::CLImage2D< T > *finalFrameReadout_ ;



    //empty
    /**
     * @brief imagesArray_
     */
    clData::CLImage2DArray< T > *imagesArray_ ;

    /**
     * @brief depthIndex_
     */
    clData::CLBuffer< uint > *depthIndex_ ;

    /**
     * @brief criticalMutex_
     */
    QMutex criticalMutex_ ;
};

}
}

#endif // CLCompositor_H
