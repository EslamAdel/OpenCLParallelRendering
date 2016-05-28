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
                       const uint64_t frameWidth = 512 ,
                       const uint64_t frameHeight = 512 );




    virtual void addCLRenderer( const uint64_t gpuIndex ) Q_DECL_OVERRIDE;



    virtual void addCLCompositor( const uint64_t gpuIndex ) Q_DECL_OVERRIDE;



    void finishedRendering_SLOT( Renderer::CLAbstractRenderer *renderer ) Q_DECL_OVERRIDE;

    void compositingFinished_SLOT( ) Q_DECL_OVERRIDE;


    void frameLoadedToDevice_SLOT( Renderer::CLAbstractRenderer *renderer ) Q_DECL_OVERRIDE;


    void pixmapReady_SLOT( QPixmap *pixmap ,
                           const Renderer::CLAbstractRenderer * renderer ) Q_DECL_OVERRIDE;


    virtual void distributeBaseVolume() Q_DECL_OVERRIDE;

    void initializeRenderers() Q_DECL_OVERRIDE;

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


protected:
    //Volume Data
    Volume< V > *baseVolume_;


};

}
}
#endif // SORTLASTRENDERER_H