#include "CommandLineParser_SortFirst.h"

CommandLineParserSortFirst::CommandLineParserSortFirst(
        const QCoreApplication &app ,
        QCommandLineParser &parser ,
        QString helpDescription )
    :  CommandLineParserBenchmark( app , parser , helpDescription )
{

}

CommandLineParser::CommandLineResult
CommandLineParserSortFirst::tokenize_sortfirst(
        Volume<uchar> *&volume,
        uint &frameWidth,
        uint &frameHeight,
        std::list<uint> &renderers,
        QString *&errorMessage,
        uint &testFrames,
        bool &loadBalancing )
{
    CommandLineParserSortFirst::addMoreDefinitions_( );

    this->parser_.process( this->app_  );

    loadBalancing = parser_.isSet( "load-balancing" ) ;

    testFrames = parser_.value( "T" ).toFloat() * 360 ;

    LoadBalancingMode loadBalancingMode_dummy;
    uint compositorIndex_dummy ;

    return this->tokenize( volume , frameWidth , frameHeight ,
                           renderers , compositorIndex_dummy ,
                           loadBalancingMode_dummy , errorMessage );

}

void CommandLineParserSortFirst::addMoreDefinitions_()
{

    QList< QCommandLineOption > options ;

    options << QCommandLineOption( QStringList() << "T" << "test-turns" ,
                                   "Specify number of complete 360 rotations"
                                   " for the benchmarking." ,
                                   "Turns-Test" ,
                                   QString::number( DEFAULT_TEST_TURNS ))

            << QCommandLineOption( QStringList() << "B" << "load-balancing" ,
                                   "If set, then dynamic load balancing is "
                                   "performed for each rendering loop. "
                                   "Note: This option is "
                                   "considered only for sort first rendering." );

    parser_.addOptions( options );
}
