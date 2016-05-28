#include <TransferFunctionWindow.h>
#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);

    TransferFunctionWindow window(0);
    window.show();

    return app.exec();
}
