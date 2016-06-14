#include <QApplication>
#include <QCommandLineParser>
#include "CommandLineParser_Benchmark.h"
#include "clparen.h"

void setTransferFunction( clparen::Parallel::CLAbstractParallelRenderer &parallelRenderer );
uint testFrames = 0 ;


int main( int argc, char *argv[] )
{
    QCoreApplication app( argc, argv );

    QCoreApplication::setApplicationName("oclUSR Benchmark");

    QCommandLineParser parser;
    CommandLineParserBenchmark myParser( app , parser , "oclUSR Benchmark Help" );

    Volume< uchar > *volume ;
    uint frameWidth , frameHeight ;
    std::list< uint > deployGPUs ;
    uint compositorGPUIndex ;
    QString *errorMessage = nullptr ;
    bool gui = false ;


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

    clparen::Parallel::SortFirstRenderer< uchar , uint8_t >
            parallelRenderer( volume , frameWidth , frameHeight );

    for( const uint rendererIndex : deployGPUs )
        parallelRenderer.addCLRenderer( rendererIndex );

    parallelRenderer.initializeRenderers();
    parallelRenderer.distributeBaseVolume();


    parallelRenderer.startRendering();
    parallelRenderer.activateRenderingKernel_SLOT(
                clparen::CLKernel::RenderingMode::RENDERING_MODE_Ultrasound );

    setTransferFunction( parallelRenderer );
    app.exec();
}



void setTransferFunction( clparen::Parallel::CLAbstractParallelRenderer &parallelRenderer )
{

    float *transferFunctionTable = new float[ 1024 ];

    for( uint32_t i = 0 ; i < 256 ; ++i )
    {
        transferFunctionTable[i * 4 + 0] = ( rand()%255 ) / float( 256 );
        transferFunctionTable[i * 4 + 1] = ( rand()%255 ) / float( 256 );
        transferFunctionTable[i * 4 + 2] = ( rand()%255 ) / float( 256 );
        transferFunctionTable[i * 4 + 3] = ( rand()%255 ) / float( 256 );
    }

    // updateKernel
    parallelRenderer.updateTransferFunction_SLOT( transferFunctionTable , 256);

    delete transferFunctionTable;
}
