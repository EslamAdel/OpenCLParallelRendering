#include <QApplication>
#include "CommandLineParser_SortFirst.h"
#include "clparen.h"

void setTransferFunction( clparen::Parallel::CLAbstractParallelRenderer &parallelRenderer );
uint testFrames = 0 ;


int main( int argc, char *argv[] )
{
    QCoreApplication app( argc, argv );

    QCoreApplication::setApplicationName("oclUSR Benchmark");

    QCommandLineParser parser;
    CommandLineParserSortFirst myParser( app , parser , "oclUSR Help" );

    Volume< uchar > *volume ;
    uint frameWidth , frameHeight ;
    std::list< uint > deployGPUs ;
    bool loadBalancing ;
    QString *errorMessage = nullptr ;

    CommandLineParser::CommandLineResult result =
            myParser.tokenize_sortfirst( volume , frameWidth , frameHeight ,
                                         deployGPUs , errorMessage, testFrames ,
                                         loadBalancing );

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
    parallelRenderer.setLoadBalancing( loadBalancing );

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
