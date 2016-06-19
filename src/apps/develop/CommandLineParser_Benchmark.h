#ifndef COMMANDLINEPARSER_BENCHMARK_H
#define COMMANDLINEPARSER_BENCHMARK_H
#include "Volume.h"
#include <QCommandLineParser>
#include "CommandLineParser.h"
#include "oclHWDL.h"

using namespace clparen::Parallel;


#define DEFAULT_TEST_TURNS 5
#define DEFAULT_TEST_FRAMES 5*360

class CommandLineParserBenchmark : public CommandLineParser
{

public:

    CommandLineParserBenchmark( const QCoreApplication &app ,
                                QCommandLineParser &parser ,
                                QString helpDescription = "oclMGPUVR Helper" );




    CommandLineResult tokenize_benchmark(
            Volume<uchar> *&volume ,
            uint &frameWidth ,
            uint &frameHeight ,
            std::list<uint> &renderers ,
            uint &compositorGPUIndex ,
            LoadBalancingMode &balancingMode ,
            QString *&errorMessage ,
            bool &gui ,
            uint &testFrames );

protected:

    virtual void addMoreDefinitions_();
};

#endif // COMMANDLINEPARSER_H
