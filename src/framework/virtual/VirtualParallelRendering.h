#ifndef VIRTUALPARALLELRENDERING_H
#define VIRTUALPARALLELRENDERING_H

#include "ParallelRendering.h"
#include "VirtualRenderingNode.h"
#include "VirtualCompositingNode.h"
#include "VirtualTaskCollect.h"



class VirtualParallelRendering : public ParallelRendering
{
    Q_OBJECT

public:
    VirtualParallelRendering( Volume< uchar >* volume ,
                              const uint frameWidth = 512 ,
                              const uint frameHeight = 512 ,
                              const uint virtualGPUsCount = 0 );


    void addRenderingNode( const uint64_t gpuIndex = 0 );


    void addCompositingNode( const uint64_t gpuIndex = 0 );


    void distributeBaseVolume1D( );


    int getRenderingNodesCount() const ;

    uint getMachineGPUsCount() const ;

    void startRendering();

public slots:
    void frameLoadedToDevice_SLOT( VirtualRenderingNode *finishedNode );

    void finishedRendering_SLOT( RenderingNode *finishedNode );

    void compositingFinished_SLOT( );

protected:

    void applyTransformation_();
private:

    //for virtual GPUs only.
    std::vector< VirtualRenderingNode* > renderingNodes_ ;
    VirtualCompositingNode *compositingNode_ ;
    const uint virtualGPUsCount_ ;

    std::vector< TaskComposite* > compositingTasks_ ;
    std::vector< VirtualTaskCollect *> collectingTasks_ ;
    std::vector< TaskRender * > renderingTasks_ ;

};

#endif // VIRTUALPARALLELRENDERING_H
