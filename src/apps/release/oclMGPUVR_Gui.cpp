#include <QApplication>
#include <iostream>
#include "Logger.h"
#include "ProfilingExterns.h"
#include "ParallelRendering.h"
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

    CommandLineParser::CommandLineResult result =
            myParser.tokenize( volume , frameWidth , frameHeight , deployGPUs ,
                               compositorGPUIndex , errorMessage );

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



    clpar::ParallelRendering parallelRenderer( volume , frameWidth , frameHeight );






    RenderingWindow_Gui rw( &parallelRenderer ) ;
    rw.show();


    return a.exec();
}
