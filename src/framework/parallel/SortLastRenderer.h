#ifndef SORTLASTRENDERER_H
#define SORTLASTRENDERER_H


#include "CLAbstractParallelRenderer.h"
#include "CLRenderer.h"
#include "CLAbstractCompositor.h"





namespace clparen {
namespace Parallel {


enum LoadBalancingMode { LOAD_BALANCING_MEMORY ,
                         LOAD_BALANCING_COMPUTATIONAL ,
                         LOAD_BALANCING_OFF };


/**
 * @brief The SortLastRenderer class
 */
template< class V, class F >
class SortLastRenderer : public CLAbstractParallelRenderer
{

public:


    /**
     * @brief SortLastRenderer
     * @param volume
     * @param frameWidth
     * @param frameHeight
     */
    SortLastRenderer(

            Volume< V >* volume ,

            const uint64_t frameWidth = 512 ,

            const uint64_t frameHeight = 512 ,

            const LoadBalancingMode loadBalancingMdoe
            = LoadBalancingMode::LOAD_BALANCING_OFF ,

            const CLData::FRAME_CHANNEL_ORDER channelOrder
            = CLData::FRAME_CHANNEL_ORDER::ORDER_INTENSITY );




    virtual void addCLRenderer( const uint64_t gpuIndex ) Q_DECL_OVERRIDE;



    virtual void addCLCompositor( const uint64_t gpuIndex ) Q_DECL_OVERRIDE;



    void finishedRendering_SLOT( uint ) Q_DECL_OVERRIDE;

    void compositingFinished_SLOT( ) Q_DECL_OVERRIDE;


    void frameLoadedToDevice_SLOT( uint ) Q_DECL_OVERRIDE;


    void pixmapReady_SLOT( QPixmap *pixmap ,
                           uint ) Q_DECL_OVERRIDE;


    virtual void distributeBaseVolume() Q_DECL_OVERRIDE;

    void initializeRenderers() Q_DECL_OVERRIDE;

protected:

    /**
     * @brief distributeBaseVolume1D
     * Distribute the baseVolume_ over the CLRenderers_ evenly based on
     * the X-axis.
     */
    virtual void distributeBaseVolume1D( ) ;

    /**
     * @brief distributeBaseVolumeWeighted
     */
    virtual void distributeBaseVolumeWeighted();

    /**
     * @brief distributeBaseVolumeMemoryWeighted
     */
    virtual void distributeBaseVolumeMemoryWeighted();

    void applyTransformation_() Q_DECL_OVERRIDE;

    void benchmark_() Q_DECL_OVERRIDE;


private:
    /**
     * @brief makePixmap_
     * @param gpuIndex
     */
    void makePixmap_( uint gpuIndex );

    /**
     * @brief collectFrame_
     * @param gpuIndex
     */
    void collectFrame_( uint gpuIndex );

    /**
     * @brief composite_
     * @param gpuIndex
     */
    void composite_( uint gpuIndex );
protected:


    //Volume Data
    /**
     * @brief baseVolume_
     */
    Volume< V > *baseVolume_;


    /**
     * @brief loadBalancingMode_
     */
    const LoadBalancingMode loadBalancingMode_;


    /**
     * @brief clCompositor_
     */
    Compositor::CLCompositor< F > *clCompositor_ ;

    /**
     * @brief clRenderers_
     */
    QMap< uint , Renderer::CLRenderer< V , F >*> clRenderers_ ;

};

}
}
#endif // SORTLASTRENDERER_H
