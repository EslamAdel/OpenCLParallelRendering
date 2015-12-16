#ifndef TASKCOMPOSITE_H
#define TASKCOMPOSITE_H

#include <QObject>
#include <QRunnable>
#include "VirtualGPU.h"
#include "VirtualImage.h"
#include "VirtualNode.h"
#include <unordered_map>

typedef std::unordered_map<VirtualNode*,VirtualImage*> NodesOutputImages;

class TaskComposite : public QObject , public QRunnable
{
    Q_OBJECT


public:
    enum CompositingMode{ AllOnce , WhatYouGet , Patch } ;

    TaskComposite( const VirtualGPU &vGPU ,
                   const CompositingMode compositingMode = CompositingMode::AllOnce);

    void insertImage( const VirtualNode* vNode, const VirtualImage *vImage );

    void run();

    NodesOutputImages &nodesOutputImages();

private:
    VirtualGPU &vGPU_;
    const CompositingMode compositingMode_;
    NodesOutputImages nodesOutputImages_;
};

#endif // TASKCOMPOSITE_H
