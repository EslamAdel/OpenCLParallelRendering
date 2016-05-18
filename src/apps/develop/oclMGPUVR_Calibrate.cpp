#include <QApplication>
#include "Logger.h"
#include <QThreadPool>
#include <TaskCalibrate.h>
#include <QObject>
#include <QDebug>
#include "CommandLineParser.h"



int main(int argc, char *argv[])
{
    QApplication a( argc , argv );

//    QApplication::setApplicationName("oclMGPUVR");

//    QCommandLineParser parser;
//    CommandLineParser myParser( a , parser , "oclMGPUVR Help" );


//    Volume< uchar > *volume ;
//    uint frameWidth , frameHeight ;
//    std::list< uint > deployGPUs ;
//    uint compositorGPUIndex ;
//    QString *errorMessage = nullptr ;

//    CommandLineParser::CommandLineResult result =
//            myParser.tokenize( volume , frameWidth , frameHeight , deployGPUs ,
//                               compositorGPUIndex , errorMessage );

//    switch( result )
//    {
//        case CommandLineParser::CommandLineError :
//        {
//            if( errorMessage != nullptr )
//                LOG_ERROR("%s", errorMessage->toStdString().c_str() );
//            else
//                LOG_ERROR("Unknown Error");
//            break ;
//        }
//    }



    clparen::Calibrator::Task::TaskCalibrate *task
            =  new clparen::Calibrator::Task::TaskCalibrate( Dimensions2D( 512 , 512 ) ,
                                                             Dimensions3D( 1024 , 1024 , 512 ) ,
                                                             1000 );


    QThreadPool::globalInstance()->start( task );



    return a.exec();
}
