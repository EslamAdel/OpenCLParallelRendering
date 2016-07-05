#ifndef CLABSTRACTPARALLELRENDERER_H
#define CLABSTRACTPARALLELRENDERER_H

#include <QObject>
#include "ProfilingExterns.h"
#include "Headers.hh"
#include <QObject>
#include <QList>
#include <QSet>
#include <QMap>
#include <QThreadPool>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>

#include <oclHWDL.h>
#include "CLVolumeVariants.hh"
#include "Transformation.h"
#include "CLAbstractCompositor.h"


namespace clparen {
namespace Parallel {


typedef QMap< uint , Renderer::CLAbstractRenderer* > Renderers;



class CLAbstractParallelRenderer : public QObject
{
    Q_OBJECT
public:
    explicit CLAbstractParallelRenderer(
            const uint64_t frameWidth = 512 ,
            const uint64_t frameHeight = 512  ,
            const CLData::FRAME_CHANNEL_ORDER channelOrder =
            CLData::FRAME_CHANNEL_ORDER::ORDER_DEFAULT ,
            QObject *parent = 0 );


    /**
     * @brief addCLRenderer
     * Create and attach CLRenderer to the GPU indexed by gpuIndex.
     *
     * @param gpuIndex
     */
    virtual void addCLRenderer( const uint64_t gpuIndex ) = 0;


    /**
     * @brief addCLCompositor
     * Create and attach CLCompositor to the GPU indexed by gpuIndex.
     * @param gpuIndex
     */
    virtual void addCLCompositor( const uint64_t gpuIndex ) = 0;


    virtual void distributeBaseVolume() = 0;

    /**
     * @brief startRendering
     * Spark the rendering loop.
     */
    virtual void startRendering( ) ;


    /**
     * @brief initializeRenderers
     */
    virtual void initializeRenderers( ) = 0 ;


    /**
     * @brief getFrameWidth
     * @return
     */
    uint64_t getFrameWidth() const ;

    /**
     * @brief getFrameHeight
     * @return
     */
    uint64_t getFrameHeight() const ;


    Compositor::CLAbstractCompositor &getCLCompositor();


    Renderer::CLAbstractRenderer &getCLRenderer( uint gpuIndex );

Q_SIGNALS:

    /**
     * @brief frameworkReady_SIGNAL
     */
    void frameworkReady_SIGNAL( );

    /**
     * @brief framesReady_SIGNAL
     * For each rendered frame done, emit a signal.
     */
    void frameReady_SIGNAL( QPixmap *pixmap ,
                            uint gpuIndex );

    /**
     * @brief finalFrameReady_SIGNAL
     * For each compositing done, emit a signal and pass
     * the collageFrame.
     *
     * @param finalFrame
     * Composited collage frame as a pixmap.
     */
    void finalFrameReady_SIGNAL( QPixmap *finalFrame );


    /**
     * @brief finishedCompositing_SIGNAL
     */
    void compositingFinished_SIGNAL( );


    /**
     * @brief finishedRendering_SIGNAL
     */
    void finishedRendering_SIGNAL( uint gpuIndex );


    /**
     * @brief frameLoadedToDevice_SIGNAL
     * @param gpuIndex
     */
    void frameLoadedToDevice_SIGNAL( uint gpuIndex );
public Q_SLOTS:

    /**
     * @brief finishedRendering_SLOT
     * @param gpuIndex
     */
    virtual void finishedRendering_SLOT( uint gpuIndex ) = 0;

    /**
     * @brief compositingFinished_SLOT
     */
    virtual void compositingFinished_SLOT( ) = 0 ;


    /**
     * @brief frameLoadedToDevice_SLOT
     * @param gpuIndex
     */
    virtual void frameLoadedToDevice_SLOT( uint gpuIndex ) = 0;



    /**
     * @brief pixmapReady_SLOT
     * @param pixmap
     * @param gpuIndex
     */
    virtual void pixmapReady_SLOT( QPixmap *pixmap ,
                                   uint gpuIndex ) = 0;

    /**
     * @brief updateRotationX_SLOT
     * @param angle
     */
    void updateRotationX_SLOT( int angle );

    /**
     * @brief updateRotationY_SLOT
     * @param angle
     */
    void updateRotationY_SLOT( int angle );

    /**
     * @brief updateRotationZ_SLOT
     * @param angle
     */
    void updateRotationZ_SLOT( int angle );

    /**
     * @brief updateTranslationX_SLOT
     * @param distance
     */
    void updateTranslationX_SLOT( int distance );

    /**
     * @brief updateTranslationY_SLOT
     * @param distance
     */
    void updateTranslationY_SLOT( int distance );
    /**
     * @brief updateTranslationZ_SLOT
     * @param distance
     */
    void updateTranslationZ_SLOT( int distance );

    /**
     * @brief updateScaleX_SLOT
     * @param distance
     */
    void updateScaleX_SLOT( int distance );
    /**
     * @brief updateScaleY_SLOT
     * @param distance
     */
    void updateScaleY_SLOT( int distance );
    /**
     * @brief updateScaleZ_SLOT
     * @param distance
     */
    void updateScaleZ_SLOT( int distance );
    /**
     * @brief updateScaleXYZ_SLOT
     * @param distance
     */
    void updateScaleXYZ_SLOT( int distance );
    /**
     * @brief updateImageBrightness_SLOT
     * @param brithness
     */
    void updateImageBrightness_SLOT( float brithness );

    /**
     * @brief updateVolumeDensity_SLOT
     * @param density
     */
    void updateVolumeDensity_SLOT( float density );

    /**
     * @brief updateMaxSteps_SLOT
     * @param value
     */
    void updateMaxSteps_SLOT( int value );

    /**
     * @brief updateStepSize_SLOT
     * @param value
     */
    void updateStepSize_SLOT( float value );

    /**
     * @brief updateApexAngle_SLOT
     * @param value
     */
    void updateApexAngle_SLOT( int value );

    /**
     * @brief updateTransferFunction_SLOT
     * @param transferFunction
     */
    void updateTransferFunction_SLOT( float *transferFunction , uint length );

    /**
     * @brief activateRenderingKernel_SLOT
     */
    /**
     * @brief updateIsoValue_SLOT
     */
    void updateIsoValue_SLOT( float isoValue );
    /**
     * @brief activateRenderingKernel_SLOT
     * @param type
     */
    void activateRenderingKernel_SLOT( clparen::CLKernel::RenderingMode type );



protected:
    /**
     * @brief applyTransformation
     * Start rendering and apply the desired transformation.
     */
    virtual void applyTransformation_() = 0;

    /**
     * @brief syncTransformation
     * Copy the transformation parameters from the mutable variables
     * everytime to the mutable parameters when threads are inactive.
     */
    void syncTransformation_();

    /**
     * @brief benchmark_
     */
    virtual void benchmark_() = 0 ;


    /**
     * @brief assertThread_
     */
    void assertThread_();


protected:
    //oclHWDl utilities
    /**
     * @brief clHardware_
     */
    oclHWDL::Hardware clHardware_;

    /**
     * @brief listGPUs_
     */
    oclHWDL::Devices listGPUs_;


    /**
     * @brief renderers_
     */
    Renderers renderers_ ;

    /**
     * @brief compositor_
     */
    Compositor::CLAbstractCompositor *compositor_;
    //threadpools
    /**
     * @brief rendererPool_
     */
    QThreadPool rendererPool_  ; //[producer] for collector pool.

    /**
     * @brief compositorPool_
     */
    QThreadPool compositorPool_; //[consumer] for collector pool.

    /**
     * @brief collectorPool_
     */
    QThreadPool collectorPool_ ; //[producer] for renderer pool AND
    //[consumer] for renderer pool.



    // Transformations
    /**
     * @brief transformation_
     */
    Transformation transformation_ ;

    // thread-safe copy
    /**
     * @brief transformationAsync_
     */
    Transformation transformationAsync_ ;

    //flags
    /**
     * @brief pendingTransformations_
     */
    bool pendingTransformations_;
    /**
     * @brief renderersReady_
     */
    bool renderersReady_;

    //counters
    uint8_t activeRenderers_;
    uint8_t compositedFramesCount_ ;


    /**
     * @brief frameWidth_
     */
    const uint64_t frameWidth_ ;

    /**
     * @brief frameHeight_
     */
    const uint64_t frameHeight_ ;


    /**
     * @brief frameChannelOrder_
     */
    const CLData::FRAME_CHANNEL_ORDER frameChannelOrder_ ;

};

}
}

#endif // CLABSTRACTPARALLELRENDERER_H
