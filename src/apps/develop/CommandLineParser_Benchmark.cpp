#include "CommandLineParser_Benchmark.h"
#include <QSet>
#include <QObject>
#include <memory>

CommandLineParserBenchmark::CommandLineParserBenchmark(
        const QCoreApplication &app ,
        QCommandLineParser &parser ,
        QString helpDescription )
    : CommandLineParser( app , parser , helpDescription )
{

}




CommandLineParser::CommandLineResult
CommandLineParserBenchmark::tokenize_benchmark(
        Volume<uchar> *&volume,
        uint &frameWidth,
        uint &frameHeight,
        std::list<uint> &renderers,
        uint &compositorGPUIndex,
        LoadBalancingMode &balancingMode ,
        QString *&errorMessage ,
        bool &gui , uint &testFrames )
{
    addMoreDefinitions_( );

    this->parser_.process( this->app_  );

    if( parser_.isSet( "gui" ) )
        gui = true ;
    else
        gui = false ;

    testFrames = parser_.value( "T" ).toFloat() * 360 ;

    return this->tokenize( volume , frameWidth , frameHeight ,
                           renderers , compositorGPUIndex ,
                           balancingMode , errorMessage );
}

void CommandLineParserBenchmark::addMoreDefinitions_()
{
    QList< QCommandLineOption > options ;

    options <<
               QCommandLineOption( QStringList() << "g" << "gui" ,
                                   "Graphical User Interface mode")

            << QCommandLineOption( QStringList() << "T" << "test-turns" ,
                                   "Specify number of complete 360 rotations"
                                   " for the benchmarking." ,
                                   "Turns-Test" ,
                                   QString::number( DEFAULT_TEST_TURNS ));

    parser_.addOptions( options );
}
