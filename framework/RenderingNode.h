#ifndef RENDERINGNODE_H
#define RENDERINGNODE_H

#include <QObject>
#include "CLContext.h"
#include "Volume.h"
#include "Image.h"



class RenderingNode : public QObject
{
    Q_OBJECT
public:
    RenderingNode( const uint64_t gpuIndex,
                   Coordinates3D &globalTranslation,
                   Coordinates3D &globalRotation,
                   float &volumeDensity,
                   float &brightness,
                   const Volume<uchar> *subVolume= nullptr );

    void loadVolume(const Volume<uchar> *subVolume);

    void applyTransformation();

    void uploadBuffer();

    void frameBufferToPixmap();

    const uint64_t & gpuIndex() const;

    bool operator==( const RenderingNode &rhs) const;

    CLContext< uint8_t > *getContext() const;

signals:
    void finishedRendering( RenderingNode *thisPtr );
//    void finishedCompositing( VirtualNode *thisPtr );
    void bufferUploaded( RenderingNode *thisPtr );



private:

    CLContext< uint8_t >* clContext_;

    const uint64_t &gpuIndex_;

    Coordinates3D &rotation_ ;
    Coordinates3D &translation_ ;
    float &volumeDensity_ ;
    float &imageBrightness_ ;



};

#endif // RENDERINGNODE_H
