#ifndef VIRTUALFRAMEWORK_H
#define VIRTUALFRAMEWORK_H
#include "VirtualNode.h"
#include "VirtualVolume.h"
#include "VirtualGPU.h"
#include <QList>
#include <QThreadPool>

class VirtualFrameWork
{
    Q_OBJECT
public :
enum CompositingMode{ AllOnce , WhatYouGet , Patch } ;
enum FrameWorkMode{ AutoTest } ;

    VirtualFrameWork( const VirtualVolume &volume ,
                      const CompositingMode compositingMode = CompositingMode::AllOnce ,
                      const FrameWorkMode frameWorkMode = FrameWorkMode::AutoTest );

    void addVirtualNode() ;

    void startRendering();
    void applyTramsformation();


private:
    void updateRendering_();
    void distributeVolume_();

public slots :
    void slotNodeFinishedRendering( VirtualNode *vNode );
    void slotNodeFinishedCompositing( VirtualNode *vNode );
    void slotNodeImageUploaded( VirtualNode *vNode);
    void slotNewTransformations();


signals :
    void blockTransformations(bool);
    void blockNewNodes(bool);

private:
    VirtualVolume mainVolume_;
    QList<VirtualNode*> nodes_;
    bool blockTransform_ ;
    VirtualGPU *serverGPU_;

    QThreadPool renderer_ ; // producer for collector
    QThreadPool collector_; // consumer for renderer, producer for compositor
    QThreadPool compositor_; // consumer for collector

};

#endif // VIRTUALFRAMEWORK_H
