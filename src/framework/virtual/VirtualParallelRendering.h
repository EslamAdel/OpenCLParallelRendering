#ifndef VIRTUALPARALLELRENDERING_H
#define VIRTUALPARALLELRENDERING_H

#include "ParallelRendering.h"
#include "VirtualCLRenderer.h"
#include "VirtualCLCompositor.h"
#include "VirtualTaskCollect.h"
#include "VirtualCLRenderer.h"



class VirtualParallelRendering : public ParallelRendering
{
    Q_OBJECT

public:
    VirtualParallelRendering( Volume< uchar >* volume ,
                              const uint frameWidth = 512 ,
                              const uint frameHeight = 512 ,
                              const uint virtualGPUsCount = 0 );


    void addCLRenderer( const uint64_t gpuIndex = 0 );


    void addCLCompositor( const uint64_t gpuIndex = 0 );


    void distributeBaseVolume1D( );


    int getCLRenderersCount() const ;

    uint getMachineGPUsCount() const ;

    void startRendering();

public slots:
    void frameLoadedToDevice_SLOT(
            CLAbstractRenderer *renderer );

    void finishedRendering_SLOT( CLAbstractRenderer *renderer );

    void compositingFinished_SLOT( );

protected:

    void applyTransformation_();
private:

    //for virtual GPUs only.
    std::vector< CLAbstractRenderer* > renderers_ ;
    CLAbstractCompositor *compositor_ ;
    const uint virtualGPUsCount_ ;

    std::vector< TaskComposite* > compositingTasks_ ;
    std::vector< VirtualTaskCollect *> collectingTasks_ ;
    std::vector< TaskRender * > renderingTasks_ ;

};

#endif // VIRTUALPARALLELRENDERING_H
