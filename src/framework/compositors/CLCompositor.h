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
    const CLData::CLFrameVariant &getFinalFrame( ) const override;

    /**
     * @brief getFinalFrame
     * @param clImage2D
     * @return
     */
    const CLData::CLImage2D< T > &getFinalFrame( bool clImage2D );

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
    CLData::CLImage2D< T > *finalFrame_ ;

    /**
     * @brief finalFrameReadout_
     */
    CLData::CLImage2D< T > *finalFrameReadout_ ;



    //empty
    /**
     * @brief imagesArray_
     */
    CLData::CLImage2DArray< T > *imagesArray_ ;

    /**
     * @brief depthIndex_
     */
    CLData::CLBuffer< uint > *depthIndex_ ;

    /**
     * @brief criticalMutex_
     */
    QMutex criticalMutex_ ;
};

}
}

#endif // CLCompositor_H
