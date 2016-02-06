#include "RenderingWindow.h"
#include "ui_RenderingWindow.h"
#include <Logger.h>
#include <QFileDialog>
#include <QDateTime>
#include <QDir>
#include <QPicture>

#define MAX_NODES 3
#define WIDTH 2048
#define HEIGHT 2048
/// TODO: Pass to arguments
#define VOLUME_PREFIX "/projects/volume-datasets/skull/skull"

RenderingWindow::RenderingWindow( QWidget *parent ) :
    QMainWindow( parent ),
    ui( new Ui::RenderingWindow )
{
    // UI setup
    ui->setupUi( this );

    LOG_INFO( "Loading volume" );
    const std::string prefix = VOLUME_PREFIX;
    Volume< uchar >* volume = new Volume< uchar >( prefix );

    LOG_DEBUG( "Creating Parallel Rendering Framework!..." );

    parallelRenderer_ = new ParallelRendering( volume , WIDTH , HEIGHT);

    frameContainers_.push_back( ui->frameContainer0 );
    frameContainers_.push_back( ui->frameContainer1 );
    frameContainers_.push_back( ui->frameContainer2 );
    for( auto i = 0 ; i < parallelRenderer_->machineGPUsCount() ; i++ )
    {
        LOG_DEBUG( "Deploy GPU#%d" , i );
        parallelRenderer_->addRenderingNode( i );
        if( i < frameContainers_.size())
            frameContainers_[ i ]->setEnabled( true );
    }

    LOG_DEBUG( "Distribute Volume" );
    parallelRenderer_->distributeBaseVolume1D();

    LOG_DEBUG("Add Compositing Node");
    parallelRenderer_->addCompositingNode( 1 );

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
                             frameContainers_[ id ]->height( ),
                             Qt::KeepAspectRatio )));


}

void RenderingWindow::frameReady_SLOT( QPixmap *frame ,
                                       const RenderingNode *node )
{
    uint index = node->getGPUIndex();

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

    QString newDir = dir + QString( "/" ) + date +
                     QString( "[%1x%2]" ).arg( QString::number( WIDTH ) ,
                                               QString::number( HEIGHT ));
    QDir createDir;
    createDir.mkdir( newDir );
    LOG_DEBUG("New Dir:%s" , newDir.toStdString().c_str() );


    QPixmap pic( finalFrame_->
                 scaledToHeight( WIDTH ).scaledToWidth( HEIGHT ));
    pic.save( newDir + "/result.jpg");

    int i = 0;
    for( const QLabel *frame : frameContainers_ )
    {
        if( frame->isEnabled() )
        {
            QPixmap framePixmap( parallelRenderer_->getRenderingNode( i ).
                                 getCLFrame()->getFramePixmap().
                                 scaledToHeight( WIDTH ).
                                 scaledToWidth( HEIGHT ));

            framePixmap.save( newDir + QString("/GPU%1.jpg").arg(i++) );
        }
    }
}

