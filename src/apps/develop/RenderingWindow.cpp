#include "RenderingWindow.h"
#include "ui_RenderingWindow.h"

#include <Logger.h>
#include <QFileDialog>
#include <QDateTime>
#include <QDir>
#include <QPicture>


RenderingWindow::RenderingWindow(
        clparen::Parallel::CLAbstractParallelRenderer *parallelRenderer ,
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
             SIGNAL( frameReady_SIGNAL(
                         QPixmap * ,
                         const clparen::Renderer::CLAbstractRenderer* )),
             this ,
             SLOT( frameReady_SLOT(
                       QPixmap * ,
                       const clparen::Renderer::CLAbstractRenderer* )));

    //    connect( parallelRenderer_ , SIGNAL( finalFrameReady_SIGNAL( QPixmap* )) ,
    //             this , SLOT( finalFrameReady_SLOT( QPixmap* )));

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
                              frameContainers_[ id ]->height( ) ,
                              Qt::KeepAspectRatio )));


}

void RenderingWindow::frameReady_SLOT(
        QPixmap *frame,
        const clparen::Renderer::CLAbstractRenderer *renderer )
{

    if(renderer == nullptr )
    {
        finalFrameReady_SLOT( frame );
        return ;
    }
    uint index = renderer->getGPUIndex();

    if( index < frameContainers_.size() )
        displayFrame_( frame , index );
}

void RenderingWindow::finalFrameReady_SLOT( QPixmap *finalFrame )
{

    //    LOG_DEBUG("Final Frame ready ");
    if( finalFrame->isNull() )
        LOG_ERROR("NULL pixmap");

    finalFrame_ = finalFrame;
    ui->frameContainerResult->
            setPixmap( finalFrame->scaled( ui->frameContainerResult->width( ) ,
                                           ui->frameContainerResult->height( ) ,
                                           Qt::KeepAspectRatio));
}

void RenderingWindow::newXRotation_SLOT(int value)
{
    ui->xRotationLabel->setText( QString::number( value ));

    parallelRenderer_->updateRotationX_SLOT( value );
}

void RenderingWindow::newYRotation_SLOT(int value)
{
    ui->yRotationLabel->setText( QString::number( value ));

    parallelRenderer_->updateRotationY_SLOT( value );
}

void RenderingWindow::newZRotation_SLOT(int value)
{
    ui->zRotationLabel->setText( QString::number( value ));

    parallelRenderer_->updateRotationZ_SLOT( value );

}

void RenderingWindow::newXTranslation_SLOT(int value)
{
    ui->xTranslationLabel->setText( QString::number( value ));

    parallelRenderer_->updateTranslationX_SLOT( value );
}

void RenderingWindow::newYTranslation_SLOT(int value)
{
    ui->yTranslationLabel->setText( QString::number( value ));

    parallelRenderer_->updateTranslationY_SLOT( value );
}

void RenderingWindow::newBrightness_SLOT(int value)
{
    ui->brightnessLabel->setText( QString::number( value ));

    float brightness =  float( value ) / 100.0;
    parallelRenderer_->updateImageBrightness_SLOT( brightness );

}

void RenderingWindow::newDensity_SLOT(int value)
{
    ui->densityLabel->setText( QString::number( value ));

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
    //            QPixmap framePixmap( parallelRenderer_->getCLRenderer( i ).
    //                                 getCLFrame()->getFramePixmap().
    //                                 scaledToHeight( FRAME_WIDTH ).
    //                                 scaledToWidth( FRAME_HEIGHT ));

    //            framePixmap.save( newDir + QString("/GPU%1.jpg").arg(i++) );
    //        }
    //    }
}

