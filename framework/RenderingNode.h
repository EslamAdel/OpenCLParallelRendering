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
                   const Volume<uchar> *subVolume= nullptr );

    void loadVolume(const Volume<uchar> *subVolume);

    void setTransformations( const Coordinates3D rotation ,
                             const Coordinates3D translation,
                             const float volumeDensity ,
                             const float imageBrightness );

    void applyTransformation();

    void uploadBuffer();

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

    Coordinates3D rotation_ ;
    Coordinates3D translation_ ;
    float volumeDensity_ ;
    float imageBrightness_ ;


};

#endif // RENDERINGNODE_H
