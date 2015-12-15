#include "TaskRender.h"


TaskRender::TaskRender( const VirtualNode &node,
                        const TaskRender::RenderingTask renderingTask,
                        const Transformation *transformation)
    : node_( node ),
      renderingTask_( renderingTask),
      transformation_( transformation )
{
    if ( renderingTask == RenderingTask::ApplyTransformation &&
         transformation == nullptr )
    {
        std::count << "Pass valid transformations!\n";
        exit(EXIT_FAILURE);
    }

}

const VirtualNode &TaskRender::node() const
{
    return node_;
}

TaskRender::RenderingTask TaskRender::renderingTask() const
{
    return renderingTask_;
}

const Transformation *TaskRender::transformation() const
{
    return transformation_;
}

void TaskRender::run()
{
    switch ( renderingTask_ )
    {
    case RenderingTask::InitialRendering :
    {
        node_.initialRendering();
        break;
    }
    case RenderingTask::ApplyTransformation:
    {
        node_.applyTransformation( transformation_ );
        break;
    }

    }

}
