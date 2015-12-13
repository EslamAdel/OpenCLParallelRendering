#ifndef VIRTUALNODE_H
#define VIRTUALNODE_H
#include "VirtualGPU.h"
#include "VirtualVolume.h"
#include "VirtualImage.h"

class VirtualNode
{
    Q_OBJECT

public:
    VirtualNode();

    void setVolume( const VirtualVolume &volume );
    bool volumeExist() const ;

    void startRendering();
    void applyTransformation();

    void uploadImage( VirtualImage newImage );

signals:
    void finishedRendering();

public slots:
    void slotFinishedRendering( VirtualGPU *finishedGPU ) ;

private :
    bool volumeExist_;
    VirtualGPU vGPU_;
    VirtualVolume subVolume_ ; //sort last

};

#endif // VIRTUALNODE_H
