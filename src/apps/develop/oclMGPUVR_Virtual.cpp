#include <QApplication>
#include <iostream>
#include "Logger.h"
#include "VirtualParallelRendering.h"
#include "ProfilingExterns.h"
#include <QCommandLineParser>
#include "CommandLineParser_Virtual.h"
#include "RenderingWindow.h"

int main(int argc, char *argv[])
{

#ifdef BENCHMARKING
    LOG_ERROR("BENCHMARKING MUST NOT BE DEFINED!");
#endif
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("oclMGPUVR_Virtual");

    QCommandLineParser parser;
    CommandLineParser_Virtual myParser( a , parser , "oclMGPUVR_Virtual Help" );


    Volume< uchar > *volume ;
    uint frameWidth , frameHeight ;
    QString *errorMessage = nullptr ;
    uint virtualRenderers ;

    CommandLineParser::CommandLineResult result =
            myParser.tokenize_virtual( volume , frameWidth , frameHeight ,
                                       virtualRenderers , errorMessage );

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

    LOG_DEBUG("Virtual Renderers = %d" , virtualRenderers );


    VirtualParallelRendering parallelRenderer( volume , frameWidth , frameHeight );



    for( uint i = 0 ; i < virtualRenderers ; i++ )
        parallelRenderer.addCLRenderer( i );

    parallelRenderer.addCLCompositor( 0 );

    parallelRenderer.distributeBaseVolume1D();

    RenderingWindow rw( &parallelRenderer ) ;
    rw.show();


    return a.exec();
}

