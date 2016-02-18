#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H
#include "Volume.h"
#include <QCommandLineParser>
#include "oclHWDL.h"

//this is a global variable used to define the transfer function channel order
//TODO don't use global variables
extern int TFChannelOrderFlage ;
#define DEFAULT_FRAME_WIDTH 512
#define DEFAULT_FRAME_HEIGHT 512
#define MAX_FRAME_DIMENSION 2048
#define DEFAULT_COMPOSITOR_INDEX 0


class CommandLineParser
{

public:
    enum CommandLineResult
    {
        CommandLineOk ,
        CommandLineError
    };

    CommandLineParser( const QCoreApplication &app ,
                       QCommandLineParser &parser ,
                       QString helpDescription = "oclMGPUVR Helper" );




    CommandLineParser::CommandLineResult tokenize( Volume<uchar> *&volume ,
                                                   uint &frameWidth ,
                                                   uint &frameHeight ,
                                                   std::list<uint> &renderers ,
                                                   uint &compositorGPUIndex ,
                                                   QString *&errorMessage );
private:

    void addDefinitions_( );

    bool checkDevicesAvailability_( QList<uint> &devices ) const;

    std::list<uint> &getAllGPUs_( ) const ;

protected:
    const QCoreApplication &app_ ;
    QCommandLineParser &parser_ ;

};

#endif // COMMANDLINEPARSER_H
