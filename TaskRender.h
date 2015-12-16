#ifndef TASKRENDER_H
#define TASKRENDER_H

#include <QRunnable>
#include <QObject>
#include <VirtualNode.h>
#include <Transformation.h>

class TaskRender : public QObject , public QRunnable
{
    Q_OBJECT
public:
    enum RenderingTask { InitialRendering , ApplyTransformation } ;

    TaskRender( const VirtualNode &node ,
                const RenderingTask renderingTask = RenderingTask::InitialRendering ,
                const Transformation *transformation = nullptr );

    void setRenderingTask( const RenderingTask renderingTask ) ;
    const VirtualNode &node() const ;
    RenderingTask renderingTask() const;
    const Transformation *transformation() const;

    void run();

signals :
    finishedRendering( TaskRender *thisPtr ) ;


private:
    const VirtualNode &node_;
    RenderingTask renderingTask_;
    const Transformation *transformation_;

};

#endif // TASKRENDER_H
