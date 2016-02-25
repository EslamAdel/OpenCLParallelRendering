#include "CommandLineParser_Virtual.h"
#include "Logger.h"


CommandLineParser_Virtual::CommandLineParser_Virtual(
        const QCoreApplication &app ,
        QCommandLineParser &parser ,
        QString helpDescription )
    : CommandLineParser( app , parser , helpDescription )
{

}

CommandLineParser::CommandLineResult
CommandLineParser_Virtual::tokenize_virtual( Volume<uchar> *&volume ,
                                             uint &frameWidth ,
                                             uint &frameHeight ,
                                             uint &virtualRenderers ,
                                             QString *&errorMessage )
{

    addMoreDefinitions_();

    this->parser_.process( this->app_ );

    if( ! parser_.isSet( "virtual" ))
    {
        errorMessage = new QString("Specify number of virtual nodes!");
        return CommandLineResult::CommandLineError ;
    }
    else
    {
        virtualRenderers = parser_.value( "virtual" ).toUInt();
        LOG_DEBUG("VirtualRenderers = %s" , parser_.value("virtual").toStdString().c_str() );
        if( virtualRenderers > MAX_VIRTUAL_RENDERERS )
        {
            errorMessage = new QString("Specify valid number of virtual nodes!");
            return CommandLineResult::CommandLineError ;
        }

    }

    std::list< uint > renderers ;
    uint compositorIndex ;
    return this->tokenize( volume ,
                           frameWidth ,
                           frameHeight ,
                           renderers ,
                           compositorIndex ,
                           errorMessage );
}

void CommandLineParser_Virtual::addMoreDefinitions_()
{
    QList< QCommandLineOption > options ;

    options <<
               QCommandLineOption( QStringList() << "i" << "virtual" ,
                                   "Number of Virutal renderers",
                                   "Virtual Renderers" );

    parser_.addOptions( options );
}
