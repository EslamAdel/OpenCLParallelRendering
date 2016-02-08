#ifndef RENDERINGNODE_H
#define RENDERINGNODE_H

#include <QObject>
#include "CLContext.h"
#include "Volume.h"

/**
 * @brief The RenderingNode class
 * This class wrapping the OpenCL Context where rendering is performed,
 * The proper use of this class is as following:
 * 1| RenderingNode::loadVolume( volume );
 * 2| RenderingNode::applyTransformation();
 * 3| RenderingNode::uploadFrameFromDevice( CL_TRUE );
 * 4| If you want to retrieve the rendered buffer as a raw bytes for further
 *    processing:
 * 4-a| RenderingNode::getFrameData();
 *    If you want to retrieve a pixmap objects for direct display:
 * 4-b| RenderingNode::getFrame();
 * 5| Repeat 2-4 for each frame.
 *
 */
class RenderingNode : public QObject , public CLContext< uchar >
{
    Q_OBJECT
public:

    /**
     * @brief RenderingNode
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
    RenderingNode( const uint64_t gpuIndex,
                   const uint frameWidth , const uint frameHeight ,
                   const Coordinates3D &globalTranslation,
                   const Coordinates3D &globalRotation,
                   const float &volumeDensity, const float &brightness  );

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

    void setFrameIndex( const uint8_t frameIndex  );

    uint8_t getFrameIndex() const ;

signals:
    /**
     * @brief finishedRendering
     * Inform the outside world if rendering is finished.
     * @param thisPtr
     * Pass this pointer as Identifier to this node. 
     */
    void finishedRendering( RenderingNode *thisPtr );




private:

    const Coordinates3D &rotation_ ;
    const Coordinates3D &translation_ ;
    const float &volumeDensity_ ;
    const float &imageBrightness_ ;


    /**
     * @brief currentCenter_
     */
    Coordinates3D currentCenter_ ;

    uint8_t frameIndex_ ;

};

#endif // RENDERINGNODE_H
