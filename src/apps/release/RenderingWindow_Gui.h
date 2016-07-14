#ifndef RENDERINGWINDOW_GUI_H
#define RENDERINGWINDOW_GUI_H


#include <QMainWindow>
#include <Headers.hh>
#include <vector>
#include <QLabel>
#include <QVector2D>
#include "SortLastRenderer.h"
#include "CLRenderer.h"
#include "ProfilingExterns.h"
#include <QThreadPool>
#include "TaskInitializeFramework.h"

namespace Ui
{
class RenderingWindow_Gui;
}


using namespace clparen;

class RenderingWindow_Gui : public QMainWindow
{
    Q_OBJECT


    enum Tabs
    {
        DevicesTabIndex = 0 ,
        TransformationTabIndex ,
        TransferFunctionTabIndex
    };


public:


    RenderingWindow_Gui( Parallel::CLAbstractParallelRenderer *parallelRenderer ,
                         QWidget *parent = 0 ) ;


    ~RenderingWindow_Gui( );

private:

    void intializeConnections_();

    /**
     * @brief startRendering_
     */
    void startRendering_( );

    void newMouseXRotation_( );

    void newMouseYRotation_( );

    void newMouseZTranslation_(int value );


    /**
     * @brief displayFrame_
     */
    void displayFrame_( QPixmap *frame , uint id );

public Q_SLOTS:
    void frameReady_SLOT( QPixmap *frame , uint index );
    void finalFrameReady_SLOT( QPixmap *finalFrame ) ;

private Q_SLOTS:

    void newXRotation_SLOT( int value );

    void newYRotation_SLOT( int value );

    void newZRotation_SLOT( int value );

    void newXTranslation_SLOT( int value );

    void newYTranslation_SLOT( int value );

    void newZTranslation_SLOT( int value );

    void newXScaling_SLOT( int value );

    void newYScaling_SLOT( int value );

    void newZScaling_SLOT( int value );

    void newXYZScaling_SLOT( int value );

    void newBrightness_SLOT( int value );

    void newDensity_SLOT( int value );

    void newIsoValue_SLOT( int value );

    void captureView_SLOT() ;

    void initializeFramework_SLOT( );

    void frameworkReady_SLOT( );

    void switchRenderingKernel_SLOT( );

    void mousePressed_SLOT( QVector2D mousePressedPosition);

    void mouseMoved_SLOT( QVector2D newPosition);

    void mouseReleased_SLOT(QVector2D releasedPosition);

    void mouseWheelMoved_SLOT(QWheelEvent* event);

private:
    Ui::RenderingWindow_Gui *ui;

    Parallel::CLAbstractParallelRenderer *parallelRenderer_ ;

    bool frameworkReady_ ;
    QVector< uint > deployGPUs_ ;
    uint compositorIndex_ ;

    QVector< QLabel*  > frameContainers_;
    QVector< QPixmap* > pixmaps_;
    QPixmap *finalFrame_;

    QVector2D lastMousePosition_;
    QVector2D mousePositionDifference_;

    float mouseXRotationAngle_;
    float mouseYRotationAngle_;

};

#endif // RENDERINGWINDOW_GUI_H
