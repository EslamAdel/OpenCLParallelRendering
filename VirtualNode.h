#ifndef VIRTUALNODE_H
#define VIRTUALNODE_H

#include <QObject>
#include "VirtualGPU.h"
#include "VirtualVolume.h"
#include "VirtualImage.h"
#include "Transformation.h"

class VirtualNode : public QObject
{
    Q_OBJECT

public:
    VirtualNode( const int id);

    void setVolume( const VirtualVolume &volume );
    bool volumeExist() const ;

    void initialRendering();
    void applyTransformation( const Transformation *transformation);

    void startRendering();

    void uploadImage();

    const VirtualImage *uploadedResultantImage() const;

    const int nodeId() const;
signals:
    void finishedRendering( VirtualNode *thisPtr );
//    void finishedCompositing( VirtualNode *thisPtr );
    void imageUploaded( VirtualNode *thisPtr );

public slots:
    void slotFinishedRendering() ;
//    void slotFinishedCompositing();


private :
    bool volumeExist_;
    const int nodeId_;
    VirtualGPU vGPU_;
    VirtualVolume subVolume_ ; //sort last
    VirtualImage *uploadedResultantImage_;
};

#endif // VIRTUALNODE_H
