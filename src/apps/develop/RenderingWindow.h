#ifndef RENDERINGWINDOW_H
#define RENDERINGWINDOW_H

#include <QtWidgets>
#include <QMainWindow>
#include <CLContext.h>
#include <Headers.hh>
#include "ParallelRendering.h"
#include "VirtualParallelRendering.h"
#include "RenderingNode.h"
#include <vector>
#include <QLabel>
#include "ProfilingExterns.h"

namespace Ui
{
class RenderingWindow;
}

class RenderingWindow : public QMainWindow
{   
    Q_OBJECT

public:

    RenderingWindow( ParallelRendering *parallelRenderer ,
                     QWidget *parent = 0 ) ;


    ~RenderingWindow( );

private:

    void intializeConnections_();

    /**
     * @brief startRendering_
     */
    void startRendering_( );

    /**
     * @brief displayFrame_
     */
    void displayFrame_( QPixmap *frame , uint id );

public slots:
    void frameReady_SLOT(QPixmap *frame, const RenderingNode *node );
    void collageFrameReady_SLOT( QPixmap *finalFrame ) ;

private slots:


    void newXRotation_SLOT( int value );



    void newYRotation_SLOT( int value );


    void newZRotation_SLOT( int value );


    void newXTranslation_SLOT( int value );


    void newYTranslation_SLOT( int value );


    void newBrightness_SLOT( int value );


    void newDensity_SLOT( int value );

    void captureView_SLOT() ;

private:
    Ui::RenderingWindow *ui;


    ParallelRendering *parallelRenderer_ ;


    QVector< QLabel*  > frameContainers_;
    QVector< QPixmap* > pixmaps_;
    QPixmap *finalFrame_;
    //std::vector
};

#endif // RENDERINGWINDOW_H
