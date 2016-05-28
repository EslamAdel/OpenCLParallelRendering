#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ColorMapWidget.h"
#include "Utilities.h"
#include <QTreeWidget>
#include <RenderingWindow.h>
#include "SortFirstRenderer.h"
#include "Headers.hh"
#include "TransferFunctionEditor.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(
            clparen::Parallel::CLAbstractParallelRenderer &parallelRenderer ,
            QWidget *parent = 0);

    ~MainWindow();


private:
    Ui::MainWindow *ui;


private slots:
    void on_xRotationSlider_valueChanged(int value);

    void on_yRotationSlider_valueChanged(int value);

    void on_zRotationSlider_valueChanged(int value);

    void on_xTranslationSlider_valueChanged(int value);

    void on_yTranslationSlider_valueChanged(int value);

    void on_xyDisplayResolutionSlider_valueChanged(int value);

    void on_xyFrameResolutionSlider_valueChanged(int value);

    void on_brightnessSlider_2_valueChanged(int value);

    void on_xyBlockDimensionSlider_valueChanged(int value);

    void on_densitySlider_valueChanged(int value);

    void on_maximumStepsSlider_valueChanged(int value);

    void on_stepSizeSlider_valueChanged(int value);
    void on_apexAngleSlider_valueChanged(int value);



private:
    /**
     * @brief startRendering_
     */
    void startRendering_( );

public:

public slots:
    /**
     * @brief displayFrame_
     */
    void displayFrame( QPixmap* frame );

private slots:

private:
    clparen::Parallel::CLAbstractParallelRenderer &parallelRenderer_ ;

signals:
    void gradientStopsChanged( const QGradientStops& stops );
    void transferFunctionChanged( UInt8Vector tf );

private Q_SLOTS:

    void _clear();
    void _load();
    void _save();
    void _setDefault();
    void _pointsUpdated();
    void _onTransferFunctionChanged( UInt8Vector tf );

    void on_xyzScaleSlider_valueChanged(int value);

private:
    void _publishTransferFunction();

private:
    ColorMapWidget* _redWidget;
    ColorMapWidget* _greenWidget;
    ColorMapWidget* _blueWidget;
    ColorMapWidget* _alphaWidget;
    float* transferFunctionTable_;
};

#endif // MAINWINDOW_H
