#include "oclUSRWindow.h"
#include "ui_oclUSRWindow.h"

oclUSRWindow::oclUSRWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::oclUSRWindow)
{
    ui->setupUi(this);
}

oclUSRWindow::~oclUSRWindow()
{
    delete ui;
}
