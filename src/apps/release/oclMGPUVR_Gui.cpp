#include <QApplication>
#include <iostream>
#include "Logger.h"
#include "ProfilingExterns.h"
#include "SortLastRenderer.h"
#include "RenderingWindow_Gui.h"
#include "CommandLineParser.h"

int main(int argc, char *argv[])
{


    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("oclMGPUVR_Gui");

    QCommandLineParser parser;
    CommandLineParser myParser( a , parser , "oclMGPUVR_Gui Help" );


    Volume< uchar > *volume ;
    uint frameWidth , frameHeight ;
    std::list< uint > deployGPUs ;
    uint compositorGPUIndex ;
    QString *errorMessage = nullptr ;
    LoadBalancingMode balancingMode;

    CommandLineParser::CommandLineResult result =
            myParser.tokenize( volume , frameWidth , frameHeight , deployGPUs ,
                               compositorGPUIndex , balancingMode , errorMessage );

    switch( result )
    {
        case CommandLineParser::CommandLineError :
        {
            if( errorMessage != nullptr )
                LOG_ERROR("%s", errorMessage->toStdString().c_str() );
            else
                LOG_ERROR("Unknown Error");
            break ;
        }
    }



    clparen::Parallel::SortLastRenderer< uchar , float >
            parallelRenderer( volume , frameWidth , frameHeight , balancingMode );






    RenderingWindow_Gui rw( &parallelRenderer ) ;
    rw.show();


    return a.exec();
}
