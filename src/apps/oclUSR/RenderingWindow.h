#ifndef RENDERINGWINDOW_H
#define RENDERINGWINDOW_H

#include <QMainWindow>
//#include <Headers.hh>

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

public:
    /**
     * @brief displayFrame_
     */
    void displayFrame( );

private slots:

private:
    Ui::RenderingWindow *ui;
};

#endif // RENDERINGWINDOW_H
