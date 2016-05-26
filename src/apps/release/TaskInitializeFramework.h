#ifndef TASKINITIALIZEFRAMEWORK_H
#define TASKINITIALIZEFRAMEWORK_H

#include <QRunnable>
#include "ParallelRendering.h"
#include <QSet>

class TaskInitializeFramework : public QRunnable
{
public:
    TaskInitializeFramework(
            clparen::Parallel::ParallelRendering *parallelRendering ,
            const QSet< uint > &renderers ,
            const uint compositor );
protected:
    void run( );

private:
    clparen::Parallel::ParallelRendering *parallelRenderer_ ;
    const QSet< uint > &renderers_ ;
    const uint compositorIndex_ ;

};

#endif // TASKINITIALIZEFRAMEWORK_H
