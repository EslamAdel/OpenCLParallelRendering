#ifndef VIRTUALNODE_H
#define VIRTUALNODE_H
#include "VirtualGPU.h"
#include "VirtualVolume.h"
#include "VirtualImage.h"
#include "Transformation.h"

class VirtualNode
{
    Q_OBJECT

public:
    VirtualNode();

    void setVolume( const VirtualVolume &volume );
    bool volumeExist() const ;

    void initialRendering();
    void applyTransformation( const Transformation *transformation);

    void startRendering();

    void uploadImage();

signals:
    void finishedRendering( VirtualNode *thisPtr );
    void finishedCompositing( VirtualNode *thisPtr );
    void imageUploaded( VirtualNode *thisPtr );

public slots:
    void slotFinishedRendering() ;
    void slotFinishedCompositing();


private :
    bool volumeExist_;
    VirtualGPU vGPU_;
    VirtualVolume subVolume_ ; //sort last
    VirtualImage *uploadedResultantImage_;
};

#endif // VIRTUALNODE_H
