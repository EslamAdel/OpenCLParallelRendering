#include "RenderingWindow.h"
#include "QApplication"
#include "MainWindow.h"
#include "TransferFunctionEditor.h"
#include "oclUSRWindow.h"
#include "CommandLineParser_SortFirst.h"

int main( int argc, char *argv[] )
{
    QApplication app( argc, argv );

    QCoreApplication::setApplicationName("oclUSR");


    QCommandLineParser parser;
    CommandLineParserSortFirst myParser( app , parser , "oclUSR Help" );

    Volume< uchar > *volume ;
    uint frameWidth , frameHeight ;
    std::list< uint > deployGPUs ;
    bool loadBalancing ;
    QString *errorMessage = nullptr ;
    uint testFrames ;

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


    MainWindow* mainWindow = new MainWindow( parallelRenderer );
    mainWindow->show();


    TransferFunctionEditor tfEditor;
    tfEditor.show();

    app.exec();
}
