#ifndef SORTFIRSTRENDERER_H
#define SORTFIRSTRENDERER_H

#include "CLAbstractParallelRenderer.h"
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QScopedPointer>
#include <QScopedArrayPointer>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>
#include <QMutexLocker>

namespace clparen
{
namespace Parallel
{

template< class V , class F >
class SortFirstRenderer : public CLAbstractParallelRenderer
{
public:

    /**
     * @brief SortLastRenderer
     * @param volume
     * @param frameWidth
     * @param frameHeight
     */
    SortFirstRenderer( Volume< V >* volume ,
                       const uint64_t frameWidth = 512 ,
                       const uint64_t frameHeight = 512 );




    virtual void addCLRenderer( const uint64_t gpuIndex ) override;



    virtual void addCLCompositor( const uint64_t gpuIndex ) override;


    virtual void distributeBaseVolume( ) override;

    void initializeRenderers( ) override;

    void finishedRendering_SLOT( Renderer::CLAbstractRenderer *renderer ) override;

    void compositingFinished_SLOT( ) override;


    void frameLoadedToDevice_SLOT( Renderer::CLAbstractRenderer *renderer ) override;


    void pixmapReady_SLOT( QPixmap *pixmap ,
                           const Renderer::CLAbstractRenderer * renderer ) override;


    void finalFrameReady_SLOT( QPixmap *pixmap )  override ;
protected:
    void applyTransformation_() override;

    void benchmark_() override;
private:
    static void render_( Renderer::CLAbstractRenderer *renderer );

    void assemble_(  Renderer::CLAbstractRenderer *renderer,
                     Image< F > *finalFrame ,
                     F *finalFrameBuffer );

    void clone_( );

protected:
    //Volume Data
    /**
     * @brief baseVolume_
     */
    QScopedPointer< Volume< V >> baseVolume_;

    /**
     * @brief frame_
     */
    QScopedPointer< Image< F >> frame_;

    /**
     * @brief frameBuffer_
     */
    QScopedArrayPointer< F > frameBuffer_ ;

    /**
     * @brief threadSafeFrame_
     */
    QScopedPointer< clData::CLImage2D< F >> clFrame_;


    /**
     * @brief frameCopyMutex_
     */
    QReadWriteLock frameCopyMutex_ ;


    /**
     * @brief framesReady_
     */
    uint renderedFramesCount_ ;

    /**
     * @brief assembledFramesCount_
     */
    uint assembledFramesCount_ ;
};



}
}

#endif // SORTFIRSTRENDERER_H
