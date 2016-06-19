#ifndef COMMANDLINEPARSER_SORTFIRST_H
#define COMMANDLINEPARSER_SORTFIRST_H

#include "CommandLineParser_Benchmark.h"

class CommandLineParserSortFirst : public CommandLineParserBenchmark
{
public:
    CommandLineParserSortFirst( const QCoreApplication &app ,
                                QCommandLineParser &parser ,
                                QString helpDescription = "oclMGPUVR Helper"  );


    CommandLineResult tokenize_sortfirst(
            Volume<uchar> *&volume ,
            uint &frameWidth ,
            uint &frameHeight ,
            std::list<uint> &renderers ,
            QString *&errorMessage ,
            uint &testFrames ,
            bool &loadBalancing );



protected:
    void addMoreDefinitions_() Q_DECL_OVERRIDE;
};

#endif // COMMANDLINEPARSER_SORTFIRST_H
