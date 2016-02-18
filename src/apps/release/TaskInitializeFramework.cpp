#include "TaskInitializeFramework.h"



TaskInitializeFramework::TaskInitializeFramework( ParallelRendering *parallelRendering,
                                                  const QSet<uint> &renderers,
                                                  const uint compositor)
    : parallelRenderer_( parallelRendering ) ,
      renderers_( renderers ),
      compositorIndex_( compositor )
{

}

void TaskInitializeFramework::run()
{
    for( const uint rendererIndex : renderers_ )
        parallelRenderer_->addRenderingNode( rendererIndex );

    parallelRenderer_->addCompositingNode( compositorIndex_ );

    parallelRenderer_->distributeBaseVolume1D();
}