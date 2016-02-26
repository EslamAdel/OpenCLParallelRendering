#ifndef CLRenderer_H
#define CLRenderer_H

#include <QObject>
#include "CLContext.h"
#include "Volume.h"
#include "Transformation.h"

/**
 * @brief The CLRenderer class
 * This class wrapping the OpenCL Context where rendering is performed,
 * The proper use of this class is as following:
 * 1| CLRenderer::loadVolume( volume );
 * 2| CLRenderer::applyTransformation();
 * 3| CLRenderer::uploadFrameFromDevice( CL_TRUE );
 * 4| If you want to retrieve the rendered buffer as a raw bytes for further
 *    processing:
 * 4-a| CLRenderer::getFrameData();
 *    If you want to retrieve a pixmap objects for direct display:
 * 4-b| CLRenderer::getFrame();
 * 5| Repeat 2-4 for each frame.
 *
 */
class CLRenderer : public QObject , public CLContext< uchar >
{
    Q_OBJECT
public:

    /**
     * @brief CLRenderer
     * @param gpuIndex
     * The index of the deployed GPU for rendering.
     * @param frameWidth
     * @param frameHeight
     * The output frame size.
     *
     * @param globalTranslation
     * @param globalRotation
     * @param volumeDensity
     * @param brightness
     * Keep tracking any transformation. pass by reference.
     * Transformation is modified outside this class, modification
     * is prohibited by 'const'ing the references, as it is accessed
     * by many threads simultaneously.
     */
    CLRenderer( const uint64_t gpuIndex,
                const uint frameWidth , const uint frameHeight ,
                const Transformation &transformation );

    /**
     * @brief applyTransformation
     * Commit any transformation and start rendering.
     */
    void applyTransformation();

    /**
     * @brief getCurrentCenter
     * @return
     */
    const Coordinates3D &getCurrentCenter() const ;

    void setFrameCenter(  const Coordinates3D center );

    void loadVolume( const Volume< uchar >* volume );

    void setFrameIndex(const uint frameIndex  );

    uint getFrameIndex() const ;

    /**
     * @brief setTransferFunctionFlag
     */
    void setTransferFunctionFlag(int flag);

    //depth comparator.
    bool operator<( const CLRenderer &rhs ) const ;

    static bool lessThan( const CLRenderer* lhs , const CLRenderer* rhs );

signals:
    /**
     * @brief finishedRendering
     * Inform the outside world if rendering is finished.
     * @param thisPtr
     * Pass this pointer as Identifier to this node.
     */
    void finishedRendering( CLRenderer *thisPtr );




private:

    const Transformation &transformation_ ;
    /**
     * @brief currentCenter_
     */
    Coordinates3D currentCenter_ ;

    uint frameIndex_ ;

};



#endif // CLRenderer_H
