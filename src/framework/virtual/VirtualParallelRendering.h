#ifndef VIRTUALPARALLELRENDERING_H
#define VIRTUALPARALLELRENDERING_H

#include "SortLastRenderer.h"
#include "VirtualCLRenderer.h"
#include "VirtualCLCompositor.h"
#include "VirtualTaskCollect.h"
#include "VirtualCLRenderer.h"


namespace clparen
{
namespace Parallel
{

class VirtualParallelRendering : public SortLastRenderer< uchar , float >
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

public Q_SLOTS:
    void frameLoadedToDevice_SLOT( Renderer::CLAbstractRenderer *renderer );

    void finishedRendering_SLOT( Renderer::CLAbstractRenderer *renderer );

    void compositingFinished_SLOT( );

protected:

    void applyTransformation_();
private:

    //for virtual GPUs only.
    std::vector< Renderer::CLAbstractRenderer* > renderers_ ;
    Compositor::CLAbstractCompositor *compositor_ ;
    const uint virtualGPUsCount_ ;

    std::vector< Compositor::Task::TaskComposite* > compositingTasks_ ;
    std::vector< Task::VirtualTaskCollect *> collectingTasks_ ;
    std::vector< Renderer::Task::TaskRender * > renderingTasks_ ;

};

}
}
#endif // VIRTUALPARALLELRENDERING_H
