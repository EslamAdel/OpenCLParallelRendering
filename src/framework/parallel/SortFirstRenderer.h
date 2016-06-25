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
    SortFirstRenderer(
            Volume< V >* volume ,
            const uint64_t frameWidth = 512 ,
            const uint64_t frameHeight = 512 ,
            const CLData::FRAME_CHANNEL_ORDER channelOrder =
            CLData::FRAME_CHANNEL_ORDER::ORDER_RGBA );




    virtual void addCLRenderer( const uint64_t gpuIndex ) Q_DECL_OVERRIDE;



    virtual void addCLCompositor( const uint64_t gpuIndex ) Q_DECL_OVERRIDE;


    virtual void distributeBaseVolume( ) Q_DECL_OVERRIDE;

    void initializeRenderers( ) Q_DECL_OVERRIDE;

    void finishedRendering_SLOT( Renderer::CLAbstractRenderer *renderer ) Q_DECL_OVERRIDE;

    void compositingFinished_SLOT( ) Q_DECL_OVERRIDE;


    void frameLoadedToDevice_SLOT( Renderer::CLAbstractRenderer *renderer ) Q_DECL_OVERRIDE;


    void pixmapReady_SLOT( QPixmap *pixmap ,
                           const Renderer::CLAbstractRenderer * renderer ) Q_DECL_OVERRIDE;



    /**
     * @brief setLoadBalancing
     * @param useLoadBalancing
     */
    void setLoadBalancing( bool useLoadBalancing );


protected:
    void applyTransformation_() Q_DECL_OVERRIDE;

    void benchmark_() Q_DECL_OVERRIDE;

    /**
     * @brief heuristicLoadBalance_
     */
    virtual void heuristicLoadBalance_();

    /**
     * @brief calculateTransferTimeMean_
     */
    void calculateTransferTimeMean_();
private:
    void render_( Renderer::CLAbstractRenderer *renderer );

    void assemble_(  Renderer::CLAbstractRenderer *renderer,
                     CLData::CLFrame< F > *finalFrame   );

    void clone_( );

    float renderingLoopTime_();

protected:
    //Volume Data
    /**
     * @brief baseVolume_
     */
    QScopedPointer< Volume< V >> baseVolume_;

    /**
     * @brief clFrame_
     */
    QScopedPointer< CLData::CLImage2D< F >> clFrame_;


    /**
     * @brief clFrameReadout_ for thread-safety considerations.
     */
    QScopedPointer< CLData::CLImage2D< F >> clFrameReadout_ ;

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

    /**
     * @brief transferTimeMean_
     */
    QMap< uint64_t , float > transferTimeMean_ ;

    /**
     * @brief renderingLoopCounter_
     */
    uint64_t renderingLoopCounter_ ;


    /**
     * @brief useLoadBalancing_
     */
    bool useLoadBalancing_ ;

};



}
}

#endif // SORTFIRSTRENDERER_H
