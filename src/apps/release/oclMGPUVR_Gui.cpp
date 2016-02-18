#include <QApplication>
#include <iostream>
#include "Logger.h"
#include "ProfilingExterns.h"
#include "ParallelRendering.h"
#include "RenderingWindow_Gui.h"
//#include "CommandLineParser.h"

int main(int argc, char *argv[])
{


    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("oclMGPUVR_Gui");

//    QCommandLineParser parser;
//    CommandLineParser myParser( a , parser , "oclMGPUVR_Gui Help" );


    Volume< uchar > *volume ;
    uint frameWidth , frameHeight ;
    std::list< uint > deployGPUs ;
    uint compositorGPUIndex ;
    QString *errorMessage = nullptr ;

//    CommandLineParser::CommandLineResult result =
//            myParser.tokenize( volume , frameWidth , frameHeight , deployGPUs ,
//                               compositorGPUIndex , errorMessage );

//    switch( result )
//    {
//        case CommandLineParser::CommandLineError :
//        {
//            if( errorMessage != nullptr )
//                LOG_ERROR("%s", errorMessage->toStdString().c_str() );
//            else
//                LOG_ERROR("Unknown Error");
//            break ;
//        }
//    }

    //    Dimensions3D size = volume->getDimensions() ;
    //    LOG_INFO("volume size: %dx%dx%d", size.x , size.y , size.z );
    //    LOG_INFO("frame: %dx%d" , frameWidth , frameHeight );
    //    for( auto index : deployGPUs )
    //        LOG_INFO("Deploy GPU<%d>", index );
    //    LOG_INFO("compositor: GPU<%d>", compositorGPUIndex );

    ParallelRendering parallelRenderer( volume , frameWidth , frameHeight );



    for( const uint rendererIndex : deployGPUs )
        parallelRenderer.addRenderingNode( rendererIndex );

    parallelRenderer.addCompositingNode( compositorGPUIndex );

    parallelRenderer.distributeBaseVolume1D();

    RenderingWindow_Gui rw( &parallelRenderer ) ;
    rw.show();


    return a.exec();
}
