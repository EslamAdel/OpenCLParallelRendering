#include "TaskRender.h"
#include <iostream>
TaskRender::TaskRender( VirtualNode &node,
                        const TaskRender::RenderingTask renderingTask,
                        const Transformation *transformation)
    : node_( node ),
      renderingTask_( renderingTask),
      transformation_( transformation )
{
    if ( renderingTask == RenderingTask::ApplyTransformation &&
         transformation == nullptr )
    {
        std::cout << "Pass valid transformations!\n";
        exit(EXIT_FAILURE);
    }

    setAutoDelete( false );

}

void TaskRender::setRenderingTask(const TaskRender::RenderingTask renderingTask)
{
    renderingTask_ = renderingTask;
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
