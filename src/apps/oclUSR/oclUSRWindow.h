#ifndef OCLUSRWINDOW_H
#define OCLUSRWINDOW_H

#include <QMainWindow>

namespace Ui {
class oclUSRWindow;
}

class oclUSRWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit oclUSRWindow(QWidget *parent = 0);
    ~oclUSRWindow();

private:
    Ui::oclUSRWindow *ui;
};

#endif // OCLUSRWINDOW_H
