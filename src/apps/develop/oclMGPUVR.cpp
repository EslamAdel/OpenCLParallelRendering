#include <QApplication>
#include <iostream>
#include "Logger.h"
#include "RenderingWindow.h"
#include "ProfilingExterns.h"
#include <QCommandLineParser>
#include "CommandLineParser.h"

int main(int argc, char *argv[])
{

#ifdef BENCHMARKING
    LOG_ERROR("BENCHMARKING MUST NOT BE DEFINED!");
#endif
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("oclMGPUVR");

    QCommandLineParser parser;
    CommandLineParser myParser( a , parser , "oclMGPUVR Help" );


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

    //    Dimensions3D size = volume->getDimensions() ;
    //    LOG_INFO("volume size: %dx%dx%d", size.x , size.y , size.z );
    //    LOG_INFO("frame: %dx%d" , frameWidth , frameHeight );
    //    for( auto index : deployGPUs )
    //        LOG_INFO("Deploy GPU<%d>", index );
    //    LOG_INFO("compositor: GPU<%d>", compositorGPUIndex );

    clpar::ParallelRendering parallelRenderer( volume , frameWidth , frameHeight );



    for( const uint rendererIndex : deployGPUs )
        parallelRenderer.addCLRenderer( rendererIndex );

    parallelRenderer.addCLCompositor( compositorGPUIndex );

//    parallelRenderer.distributeBaseVolumeMemoryWeighted();
    parallelRenderer.distributeBaseVolume1D();

    RenderingWindow rw( &parallelRenderer ) ;
    rw.show();


    return a.exec();
}
