#ifndef COMMANDLINEPARSER_VIRTUAL_H
#define COMMANDLINEPARSER_VIRTUAL_H

#include "CommandLineParser.h"
#define MAX_VIRTUAL_RENDERERS 50

class CommandLineParser_Virtual : public CommandLineParser
{

public:

    CommandLineParser_Virtual( const QCoreApplication &app ,
                               QCommandLineParser &parser ,
                               QString helpDescription = "oclMGPUVR Helper" );




    CommandLineResult tokenize_virtual( Volume<uchar> *&volume ,
                                        uint &frameWidth ,
                                        uint &frameHeight ,
                                        uint &virtualRenderers,
                                        QString *&errorMessage );



private:

    void addMoreDefinitions_();
};

#endif // COMMANDLINEPARSER_VIRTUAL_H
