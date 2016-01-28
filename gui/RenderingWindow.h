#ifndef RENDERINGWINDOW_H
#define RENDERINGWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QMainWindow>
#include <CLContext.h>
#include <Headers.hh>
#include "ParallelRendering.h"
#include "RenderingNode.h"
#include <vector>
#include <QLabel>

namespace Ui
{
class RenderingWindow;
}

class RenderingWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RenderingWindow( QWidget *parent = 0 );
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
    void displayFrame_( );

public slots:
    void framesReady_SLOT();


private slots:


    void newXRotation_SLOT( int value );



    void newYRotation_SLOT( int value );


    void newZRotation_SLOT( int value );


    void newXTranslation_SLOT( int value );


    void newYTranslation_SLOT( int value );


    void newBrightness_SLOT( int value );


    void newDensity_SLOT( int value );


private:
    Ui::RenderingWindow *ui;
   // CLContext< uint8_t >* clContext_;
    ParallelRendering *parallelRenderer_;
    QVector< QLabel*  > frameContainers_;
    QVector< QPixmap* > pixmaps_;
    //std::vector
};

#endif // RENDERINGWINDOW_H
