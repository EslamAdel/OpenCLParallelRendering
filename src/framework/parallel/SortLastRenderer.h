#ifndef SORTLASTRENDERER_H
#define SORTLASTRENDERER_H


#include "CLAbstractParallelRenderer.h"
#include "CLRenderer.h"
#include "CLAbstractCompositor.h"





namespace clparen {
namespace Parallel {




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
    SortLastRenderer( Volume< V >* volume ,
                       const uint frameWidth = 512 ,
                       const uint frameHeight = 512 );




    virtual void addCLRenderer( const uint64_t gpuIndex ) override;



    virtual void addCLCompositor( const uint64_t gpuIndex ) override;


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


    void finishedRendering_SLOT( Renderer::CLAbstractRenderer *renderer ) override;

    void compositingFinished_SLOT( ) override;


    void frameLoadedToDevice_SLOT( Renderer::CLAbstractRenderer *renderer ) override;


    void pixmapReady_SLOT( QPixmap *pixmap ,
                           const Renderer::CLAbstractRenderer * renderer ) override;



protected:
    //Volume Data
    Volume< V > *baseVolume_;


};

}
}
#endif // SORTLASTRENDERER_H
