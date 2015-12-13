#ifndef VIRTUALFRAMEWORK_H
#define VIRTUALFRAMEWORK_H
#include "VirtualNode.h"
#include "VirtualVolume.h"
#include <QList>
#include <QThreadPool>

class VirtualFrameWork
{
public :
enum CompositingMode{ AllOnce , WhatYouGet , Patch } ;
enum FrameWorkMode{ AutoTest } ;

    VirtualFrameWork( const VirtualVolume &volume ,
                      const CompositingMode compositingMode = CompositingMode::AllOnce ,
                      const FrameWorkMode frameWorkMode = FrameWorkMode::AutoTest );


    void addVirtualNode() ;



private:

    void distributeVolume_();
    void distributeTransformations_();

public slots :
    void slotNodeFinished( VirtualNode* vNode );

    void slotNewTransformations();


signals :
    void blockTransformations(bool);

    void blockNewNodes(bool);
private:
    VirtualVolume mainVolume_;
    QList<VirtualNode*> nodes_;
    bool blockTransform_ ;


    QThreadPool transformationsDistributer_; // consumer for new transformations applied

    QThreadPool compositor_; // consumer for collector
    QThreadPool collector_; // producer for compositor

};

#endif // VIRTUALFRAMEWORK_H
