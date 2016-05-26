#ifndef TASKINITIALIZEFRAMEWORK_H
#define TASKINITIALIZEFRAMEWORK_H

#include <QRunnable>
#include "SortLastRenderer.h"
#include <QSet>

class TaskInitializeFramework : public QRunnable
{
public:
    TaskInitializeFramework(
            clparen::Parallel::CLAbstractParallelRenderer *parallelRendering ,
            const QSet< uint > &renderers ,
            const uint compositor );
protected:
    void run( );

private:
    clparen::Parallel::CLAbstractParallelRenderer *parallelRenderer_ ;
    const QSet< uint > &renderers_ ;
    const uint compositorIndex_ ;

};

#endif // TASKINITIALIZEFRAMEWORK_H
