#include "CommandLineParser.h"
#include <QSet>
#include <QObject>
#include <memory>


CommandLineParser::CommandLineParser( const QCoreApplication &app ,
                                      QCommandLineParser &parser ,
                                      QString helpDescription )
    : app_( app ) ,
      parser_( parser )
{

    parser.setApplicationDescription( helpDescription );
    parser.addHelpOption();
    addDefinitions_();
}

void CommandLineParser::addDefinitions_( )
{
    parser_.setSingleDashWordOptionMode(
                QCommandLineParser::ParseAsLongOptions );


    QList< QCommandLineOption > options ;

    options << //Add width option
               QCommandLineOption( QStringList() << "v" << "volume-prefix",
                                   "Volume prefix that prepends "
                                   "the .img and .hdr files."  ,
                                   "Volume Prefix" )
            <<
               QCommandLineOption( QStringList() << "W" <<  "frame-width",
                                   "Frame Width of the rendered frame and "
                                   "final frame as well. if not set, "
                                   "the default width will be set" ,
                                   "Frame Width" ,
                                   QString::number( DEFAULT_FRAME_WIDTH ))

            << //Add Height option
               QCommandLineOption( QStringList() << "H" << "frame-height" ,
                                   "Frame Height of the rendered frame and "
                                   "final frame as well. if not set, "
                                   "the default height will be set" ,
                                   "Frame Height" ,
                                   QString::number( DEFAULT_FRAME_HEIGHT ))

            << //Add renderer(s) indices.
               QCommandLineOption( QStringList() << "r" << "renderer-index",
                                   "GPU Index to deploy as "
                                   "(r)enderer, "
                                   "GPU index starts from 0 in the "
                                   "same order as traversed "
                                   "by OpenCL device queries. "
                                   "If this option is not set"
                                   ", all available devices "
                                   "will be deployed. For multi, and not all, "
                                   "GPUs deployment, use this option as many "
                                   "as GPUs of interest "
                                   "followed by each index." ,
                                   "Renderer Index" )

            <<
               QCommandLineOption( QStringList() << "c" << "compositor-index" ,
                                   "GPU Index to deploy "
                                   "as (c)compositor, "
                                   "starting from 0 in "
                                   "the same order as "
                                   "traversed by OpenCL "
                                   "device queries. "
                                   "If this option is not "
                                   "set, default GPU index "
                                   "will be deployed." ,
                                   "Compositor Index" ,
                                   QString::number( DEFAULT_COMPOSITOR_INDEX ));

    parser_.addOptions( options );

}

bool
CommandLineParser::checkDevicesAvailability_( QList< uint > &devices ) const
{
    // Scan the hardware
    std::unique_ptr< oclHWDL::Hardware >clHardware( new oclHWDL::Hardware );

    // Get a list of all the GPUs that are connected to the system
    const oclHWDL::Devices listGPUs = clHardware.get()->getListGPUs();

    if( listGPUs.empty() ) return false ;

    for( uint index : devices )
        if( index < 0 || index > listGPUs.size() - 1 )
            return false ;

    return true ;

}

std::list< uint > &CommandLineParser::getAllGPUs_() const
{
    // Scan the hardware
    oclHWDL::Hardware* clHardware = new oclHWDL::Hardware();

    // Get a list of all the GPUs that are connected to the system
    auto countGPUs =   clHardware->getListGPUs( ).size();

    delete clHardware ;

    auto listGPUs = new std::list< uint >( ) ;
    for( uint index = 0 ; index < countGPUs ; index++ )
        listGPUs->push_back( index );

    return *listGPUs ;
}

CommandLineParser::CommandLineResult
CommandLineParser::tokenize( Volume<uchar> *&volume ,
                             uint &frameWidth ,
                             uint &frameHeight ,
                             std::list< uint > &renderers ,
                             uint &compositorGPUIndex ,
                             QString *&errorMessage )
{
    parser_.process( app_ );

    QString volumePrefix = parser_.value( "volume-prefix" );
    //collect volume prefix.
    if( ! parser_.isSet( "volume-prefix" ))
    {
        errorMessage = new QString("Volume Prefix is not set!");
        return CommandLineResult::CommandLineError ;
    }
    else
        volume = new Volume< uchar >( volumePrefix.toStdString( ));


    //collect frame width/height ... if not set, the defualt values is used.
    frameWidth = parser_.value( "frame-width" ).toUInt( );
    frameHeight = parser_.value( "frame-height" ).toUInt( );

    if( frameWidth > MAX_FRAME_DIMENSION || frameHeight > MAX_FRAME_DIMENSION )
    {
        errorMessage = new QString("Frame Dimensions exceeded 2048x2048");
        return CommandLineResult::CommandLineError ;

    }

    if( parser_.isSet( "renderer-index" ))
    {
        QStringList renderersStdLst = parser_.values( "renderer-index" );
        QSet< uint > renderersSet ;
        for( const QString rendererStr : renderersStdLst )
            renderersSet << rendererStr.toUInt() ;

        QList< uint > renderersList = renderersSet.toList();

        if( checkDevicesAvailability_( renderersList ) == false )
        {
            errorMessage = new QString("Bad GPU index.");
            return CommandLineResult::CommandLineError ;
        }

        renderers = renderersList.toStdList() ;
    }
    else
        renderers = getAllGPUs_( );

    compositorGPUIndex = parser_.value( "compositor-index" ).toUInt( );

    if( ! checkDevicesAvailability_( QList<uint>( ) << compositorGPUIndex ))
    {
        errorMessage = new QString("Bad GPU index");
        return CommandLineResult::CommandLineError ;
    }

    return CommandLineResult::CommandLineOk ;
}
