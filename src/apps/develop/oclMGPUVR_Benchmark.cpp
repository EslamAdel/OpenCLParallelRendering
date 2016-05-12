#include <QApplication>
#include <iostream>
#include "Logger.h"
#include "RenderingWindow.h"
#include "ProfilingExterns.h"
#include "CommandLineParser_Benchmark.h"


uint testFrames = 0 ;


int main(int argc, char *argv[])
{
#ifndef BENCHMARKING
    LOG_ERROR("BENCHMARKING MUST BE DEFINED");
#endif
    QCoreApplication a(argc, argv);

    QCoreApplication::setApplicationName("oclMGPUVR_benchmark");

    QCommandLineParser parser;
    CommandLineParserBenchmark myParser( a , parser , "oclMGPUVR_benchmark Help" );


    Volume< uchar > *volume ;
    uint frameWidth , frameHeight ;
    std::list< uint > deployGPUs ;
    uint compositorGPUIndex ;
    QString *errorMessage = nullptr ;
    bool gui = false ;
    //uint testFrames = 0 ;

    CommandLineParser::CommandLineResult result =
            myParser.tokenize_benchmark( volume , frameWidth , frameHeight ,
                                         deployGPUs , compositorGPUIndex ,
                                         errorMessage , gui , testFrames );

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

    clparen::ParallelRendering parallelRenderer( volume , frameWidth , frameHeight );

    for( const uint rendererIndex : deployGPUs )
        parallelRenderer.addCLRenderer( rendererIndex );

    parallelRenderer.addCLCompositor( compositorGPUIndex );

    parallelRenderer.distributeBaseVolumeMemoryWeighted();



    parallelRenderer.startRendering();

    return a.exec();
}
