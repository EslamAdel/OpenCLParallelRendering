#include <QApplication>
#include <iostream>
#include "Logger.h"
#include "RenderingWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RenderingWindow rw;
    rw.show();

    return a.exec();
}

