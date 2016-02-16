#include "RenderingWindow.h"
#include "ui_RenderingWindow.h"
#include <Logger.h>
#include <QFileDialog>
#include <QDateTime>
#include <QDir>
#include <QPicture>


RenderingWindow::RenderingWindow( ParallelRendering *parallelRenderer ,
                                  QWidget *parent )
    : QMainWindow( parent ),
      ui( new Ui::RenderingWindow )
{
    parallelRenderer_ = parallelRenderer ;


    // UI setup
    ui->setupUi( this );



    LOG_DEBUG( "Creating Parallel Rendering Framework!..." );

    QVector< QLabel* > labels ;

//    ui->frameContainer0->setEnabled( false );
    frameContainers_.push_back( ui->frameContainer0 );
    labels.push_back( ui->labelGPU0 );

//    ui->frameContainer1->setEnabled( false );
    frameContainers_.push_back( ui->frameContainer1 );
    labels.push_back( ui->labelGPU1 );

//    ui->frameContainer2->setEnabled( false );
    frameContainers_.push_back( ui->frameContainer2 );
    labels.push_back( ui->labelGPU2 );


    intializeConnections_();


    LOG_INFO( "Triggering rendering" );
    startRendering_( );
}

RenderingWindow::~RenderingWindow( )
{
    delete ui;
}

void RenderingWindow::intializeConnections_()
{
    //parallelRenderer_
    connect( parallelRenderer_ ,
             SIGNAL( frameReady_SIGNAL( QPixmap * ,
                                        const RenderingNode* )),
             this , SLOT( frameReady_SLOT( QPixmap * ,
                                           const RenderingNode* )));

    connect( parallelRenderer_ , SIGNAL( finalFrameReady_SIGNAL( QPixmap* )) ,
             this , SLOT( collageFrameReady_SLOT( QPixmap* )));


    //sliders
    connect( ui->xRotationSlider , SIGNAL( valueChanged( int )),
             this , SLOT( newXRotation_SLOT( int )));

    connect( ui->yRotationSlider , SIGNAL( valueChanged( int )),
             this , SLOT( newYRotation_SLOT( int )));

    connect( ui->zRotationSlider , SIGNAL( valueChanged( int )),
             this , SLOT( newZRotation_SLOT( int )));

    connect( ui->xTranslationSlider , SIGNAL( valueChanged( int )),
             this , SLOT( newXTranslation_SLOT( int )));

    connect( ui->yTranslationSlider , SIGNAL( valueChanged( int )),
             this , SLOT( newYTranslation_SLOT( int )));

    connect( ui->zTranslationSlider , SIGNAL( valueChanged( int )),
             this , SLOT( newZTranslation_SLOT( int )));

    connect( ui->xScalingSlider , SIGNAL( valueChanged( int )),
             this , SLOT( newYScaling_SLOT( int )));

    connect( ui->yScalingSlider , SIGNAL( valueChanged( int )),
             this , SLOT( newYScaling_SLOT( int )));

    connect( ui->zScalingSlider , SIGNAL( valueChanged( int )),
             this , SLOT( newYScaling_SLOT( int )));

    connect( ui->xyzScalingSlider , SIGNAL( valueChanged( int )),
             this , SLOT( newYScaling_SLOT( int )));

    connect( ui->brightnessSlider , SIGNAL( valueChanged( int )),
             this , SLOT( newBrightness_SLOT( int )));

    connect( ui->densitySlider , SIGNAL( valueChanged( int )),
             this , SLOT( newDensity_SLOT( int )));

    //capture button
    connect( ui->captureButton , SIGNAL( released( )) ,
             this , SLOT( captureView_SLOT( )));
}

void RenderingWindow::startRendering_( )
{
    //set slider ranges
    ui->xTranslationSlider->setRange(-5,5);
    ui->xTranslationSlider->setValue(0);
    ui->yTranslationSlider->setRange(-5,5);
    ui->yTranslationSlider->setValue(0);
    ui->zTranslationSlider->setRange(-5,5);
    ui->zTranslationSlider->setValue(0);

    ui->xRotationSlider->setRange(0,360);
    ui->xRotationSlider->setValue(0);
    ui->yRotationSlider->setRange(0,360);
    ui->yRotationSlider->setValue(0);
    ui->zRotationSlider->setRange(0,360);
    ui->zRotationSlider->setValue(0);


    ui->xScalingSlider->setRange(1,10);
    ui->xScalingSlider->setValue(1);
    ui->xScalingValue->setText(QString::number(1) );
    ui->yScalingSlider->setRange(1,10);
    ui->yScalingSlider->setValue(1);
    ui->yScalingValue->setText(QString::number(1) );
    ui->zScalingSlider->setRange(1,10);
    ui->zScalingSlider->setValue(1);
    ui->zScalingValue->setText(QString::number(1) );
    ui->xyzScalingSlider->setRange(1,10);
    ui->xyzScalingSlider->setValue(1);
    ui->xyzScalingValue->setText(QString::number(1) );

    ui->brightnessSlider->setRange(0,300);
    ui->brightnessSlider->setValue(150);
    ui->brightnessValue->setText(QString::number(150));
    ui->densitySlider->setRange(0,100);
    ui->densitySlider->setValue(50);
    ui->densityValue->setText(QString::number(50));


    // Getting the initial values from the sliders
    newXTranslation_SLOT( ui->xTranslationSlider->value( ));
    newYTranslation_SLOT( ui->yTranslationSlider->value( ));

    newXRotation_SLOT( ui->xRotationSlider->value( ));
    newYRotation_SLOT( ui->yRotationSlider->value( ));
    newZRotation_SLOT( ui->zRotationSlider->value( ));

    newBrightness_SLOT( ui->brightnessSlider->value( ));
    newDensity_SLOT( ui->densitySlider->value( ));


    parallelRenderer_->startRendering( );

}

void RenderingWindow::displayFrame_( QPixmap *frame , uint id )
{

    frameContainers_[ id ]->setPixmap
            (( frame->scaled( frameContainers_[ id ]->width( ),
                              frameContainers_[ id ]->height( ),
                              Qt::KeepAspectRatio )));


}

void RenderingWindow::frameReady_SLOT( QPixmap *frame ,
                                       const RenderingNode *node )
{
    uint index = node->getFrameIndex();

    if( index < frameContainers_.size() )
        displayFrame_( frame , index );
}

void RenderingWindow::collageFrameReady_SLOT( QPixmap *finalFrame )
{
    finalFrame_ = finalFrame;
    ui->frameContainerResult->
            setPixmap( finalFrame->scaled( ui->frameContainerResult->width( ) ,
                                           ui->frameContainerResult->height( ) ,
                                           Qt::KeepAspectRatio ));
}

void RenderingWindow::newXRotation_SLOT(int value)
{
    ui->xRotationValue->setText(QString::number(value));

    parallelRenderer_->updateRotationX_SLOT( value );
}

void RenderingWindow::newYRotation_SLOT(int value)
{
    ui->yRotationValue->setText(QString::number(value));

    parallelRenderer_->updateRotationY_SLOT( value );
}

void RenderingWindow::newZRotation_SLOT(int value)
{
    ui->zRotationValue->setText( QString::number( value ));

    parallelRenderer_->updateRotationZ_SLOT( value );

}

void RenderingWindow::newXTranslation_SLOT(int value)
{
    ui->xTranslationValue->setText( QString::number( value ));

    parallelRenderer_->updateTranslationX_SLOT( value );
}

void RenderingWindow::newYTranslation_SLOT(int value)
{
    ui->yTranslationValue->setText( QString::number( value ));

    parallelRenderer_->updateTranslationY_SLOT( value );
}

void RenderingWindow::newZTranslation_SLOT(int value)
{
    ui->zTranslationValue->setText( QString::number( value ));
    parallelRenderer_->updateTranslationZ_SLOT( value );

}

void RenderingWindow::newXScaling_SLOT(int value)
{
    ui->xScalingValue->setText( QString::number( value ));
}

void RenderingWindow::newYScaling_SLOT(int value)
{
    ui->yScalingValue->setText( QString::number( value ));
}

void RenderingWindow::newZScaling_SLOT(int value)
{
    ui->zScalingValue->setText( QString::number( value ));
}

void RenderingWindow::newXYZScaling_SLOT(int value)
{
    ui->xyzScalingValue->setText( QString::number( value ));
}



void RenderingWindow::newBrightness_SLOT(int value)
{
    ui->brightnessValue->setText( QString::number( value ));

    float brightness =  float( value ) / 100.0;
    parallelRenderer_->updateImageBrightness_SLOT( brightness );

}

void RenderingWindow::newDensity_SLOT(int value)
{
    ui->densityValue->setText( QString::number( value ));

    float density = float( value ) / 100.0;
    parallelRenderer_->updateVolumeDensity_SLOT( density );
}

void RenderingWindow::captureView_SLOT()
{
    QString dir =
            QFileDialog::
            getExistingDirectory( this , tr( "Open Directory" ) ,
                                  "/home" ,
                                  QFileDialog::ShowDirsOnly
                                  | QFileDialog::DontResolveSymlinks);


    QString date = QDateTime::currentDateTime().toString( "hh-mm-ss" );

//    QString newDir = dir + QString( "/" ) + date +
//                     QString( "[%1x%2]" ).arg( QString::number( FRAME_WIDTH ) ,
//                                               QString::number( FRAME_HEIGHT ));
//    QDir createDir;
//    createDir.mkdir( newDir );
//    LOG_DEBUG("New Dir:%s" , newDir.toStdString().c_str() );


//    QPixmap pic( finalFrame_->
//                 scaledToHeight( FRAME_WIDTH ).scaledToWidth( FRAME_HEIGHT ));
//    pic.save( newDir + "/result.jpg");

//    int i = 0;
//    for( const QLabel *frame : frameContainers_ )
//    {
//        if( frame->isEnabled() )
//        {
//            QPixmap framePixmap( parallelRenderer_->getRenderingNode( i ).
//                                 getCLFrame()->getFramePixmap().
//                                 scaledToHeight( FRAME_WIDTH ).
//                                 scaledToWidth( FRAME_HEIGHT ));

//            framePixmap.save( newDir + QString("/GPU%1.jpg").arg(i++) );
//        }
//    }
}
