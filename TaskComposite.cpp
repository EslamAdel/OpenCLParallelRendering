#include "TaskComposite.h"

TaskComposite::TaskComposite(const VirtualGPU &vGPU,
                             const CompositingMode compositingMode)
    : vGPU_( vGPU ),
      compositingMode_( compositingMode )
{


}

void TaskComposite::insertImage(const VirtualNode *vNode, const VirtualImage *vImage)
{
    nodesOutputImages_[ vNode ] = vImage ;
}

void TaskComposite::run()
{

}

NodesOutputImages &TaskComposite::nodesOutputImages()
{
    return nodesOutputImages_;
}

