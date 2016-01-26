#ifndef RENDERINGWINDOW_H
#define RENDERINGWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QMainWindow>
#include <CLContext.h>
#include <Headers.hh>
#include "ParallelRendering.h"
#include "RenderingNode.h"
#include <vector>

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

    /**
     * @brief on_xSlider_valueChanged
     * @param value
     */
    void on_xSlider_valueChanged( int value );

    /**
     * @brief on_ySlider_valueChanged
     * @param value
     */
    void on_ySlider_valueChanged( int value );

    /**
     * @brief on_zSlider_valueChanged
     * @param value
     */
    void on_zSlider_valueChanged( int value );

    /**
     * @brief on_xTranslationSlider_valueChanged
     * @param value
     */
    void on_xTranslationSlider_valueChanged( int value );

    /**
     * @brief on_yTranslationSlider_valueChanged
     * @param value
     */
    void on_yTranslationSlider_valueChanged( int value );

    /**
     * @brief on_brightnessSlider_valueChanged
     * @param value
     */
    void on_brightnessSlider_valueChanged( int value );

    /**
     * @brief on_densitySlider_valueChanged
     * @param value
     */
    void on_densitySlider_valueChanged( int value );

private:
    Ui::RenderingWindow *ui;
   // CLContext< uint8_t >* clContext_;
    ParallelRendering *parallelRenderer_;
    QVector< QPixmap* > pixmaps_;
    //std::vector
};

#endif // RENDERINGWINDOW_H
