#include <QCoreApplication>
#include <iostream>
#include "Logger.h"

#define DEMO
#include "VirtualFrameWork.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


#ifdef DEMO
    VirtualVolume v( 512 , 512 , 512 );
    LOG_INFO("Creating the virtual framework");
    VirtualFrameWork fw( v );
#endif

    return a.exec();
}

