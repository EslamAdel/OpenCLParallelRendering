#include "RenderingWindow.h"
#include "QApplication"
#include "MainWindow.h"
#include "TransferFunctionEditor.h"
#include "oclUSRWindow.h"
#include <QCommandLineParser>
#include "CommandLineParser.h"

int main( int argc, char *argv[] )
{
    QApplication app( argc, argv );

    QCoreApplication::setApplicationName("oclUSR");

    QCommandLineParser parser;
    CommandLineParser myParser( app , parser , "oclUSR Help" );

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

    clparen::Parallel::SortFirstRenderer< uchar , uint8_t >
            parallelRenderer( volume , frameWidth , frameHeight );

    for( const uint rendererIndex : deployGPUs )
        parallelRenderer.addCLRenderer( rendererIndex );

    parallelRenderer.initializeRenderers();
    parallelRenderer.distributeBaseVolume();



    MainWindow* mainWindow = new MainWindow( parallelRenderer );
    mainWindow->show();


    TransferFunctionEditor tfEditor;
    tfEditor.show();

    app.exec();
}
