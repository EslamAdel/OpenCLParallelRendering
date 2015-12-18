#include <QCoreApplication>
#include <iostream>

#define DEMO
#include "VirtualFrameWork.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


#ifdef DEMO
    VirtualVolume v( 512 , 512 , 512 );
    std::cout<<"Creating the virtual framework"<<std::endl;
    VirtualFrameWork fw( v );
#endif

    return a.exec();
}

