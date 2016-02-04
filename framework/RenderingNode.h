#ifndef RENDERINGNODE_H
#define RENDERINGNODE_H

#include <QObject>
#include "CLContext.h"
#include "Volume.h"


class RenderingNode : public QObject , public CLContext< uchar >
{
    Q_OBJECT
public:
    RenderingNode( const uint64_t gpuIndex,
                   const uint frameWidth , const uint frameHeight ,
                   Coordinates3D &globalTranslation,
                   Coordinates3D &globalRotation,
                   float &volumeDensity, float &brightness,
                   const Volume< uchar > *subVolume= nullptr );

    void applyTransformation();

signals:
    void finishedRendering( RenderingNode *thisPtr );
    void bufferUploaded( RenderingNode *thisPtr );



private:

    Coordinates3D &rotation_ ;
    Coordinates3D &translation_ ;
    float &volumeDensity_ ;
    float &imageBrightness_ ;



};

#endif // RENDERINGNODE_H
