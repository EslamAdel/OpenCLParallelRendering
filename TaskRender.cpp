#include "TaskRender.h"
#include <iostream>
#include <QThread>
#include "Logger.h"

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
        LOG_ERROR("Pass valid transformations!");
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

    LOG_INFO("From thread:{%d},\n"
             "\trendering on node:<%d> ...",
             QThread::currentThread(),
             node_.nodeId());

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

    LOG_INFO("From thread:{%d},\n"
             "\trendering is DONE on node:<%d>",
             QThread::currentThread(),
             node_.nodeId());



}
