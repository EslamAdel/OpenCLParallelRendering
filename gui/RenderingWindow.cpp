#include "RenderingWindow.h"
#include "ui_RenderingWindow.h"
#include <Logger.h>
#include <QFileDialog>
#include <QDateTime>
#include <QDir>
#include <QPicture>


#define WIDTH 512
#define HEIGHT 512
/// TODO: Pass to arguments


//IMPORTANT: Check the MAX_ALLOC_SIZE first. (using clinfo)

//in bytes: 15.6 MB
#if  VOLUME_SIZE == 256
#define VOLUME_PREFIX "/projects/volume-datasets/skull/skull"

//in bytes: 52.7 MB
#elif VOLUME_SIZE == 384
#define VOLUME_PREFIX "/projects/volume-datasets/skull-scales/skull-384-384-384/output-voulme"

//in bytes: 125 MB
#elif VOLUME_SIZE == 512
#define VOLUME_PREFIX "/projects/volume-datasets/skull-scales/skull-512-512-512/output-voulme"

//in bytes: 244.1 MB
#elif VOLUME_SIZE == 640
#define VOLUME_PREFIX "/projects/volume-datasets/skull-scales/skull-640-640-640/output-voulme"

//in bytes: 421.8 MB
#elif VOLUME_SIZE == 768
#define VOLUME_PREFIX "/projects/volume-datasets/skull-scales/skull-768-768-768/output-voulme"

//in bytes: 670 MB
#elif VOLUME_SIZE == 896
#define VOLUME_PREFIX "/projects/volume-datasets/skull-scales/skull-896-896-896/output-voulme"

//in bytes: 1.0 GB
#elif VOLUME_SIZE == 1024
#define VOLUME_PREFIX "/projects/volume-datasets/skull-scales/skull-1024-1024-1024/output-voulme"


#endif





#define min( a , b ) ( a < b )? a : b

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

    QVector< QLabel* > labels ;

    ui->frameContainer0->setEnabled( false );
    frameContainers_.push_back( ui->frameContainer0 );
    labels.push_back( ui->labelGPU0 );

    ui->frameContainer1->setEnabled( false );
    frameContainers_.push_back( ui->frameContainer1 );
    labels.push_back( ui->labelGPU1 );

    ui->frameContainer2->setEnabled( false );
    frameContainers_.push_back( ui->frameContainer2 );
    labels.push_back( ui->labelGPU2 );


#ifdef VIRTUAL_GPUS
    //argument 3 is count of virtual GPUs.
    parallelRenderer_ = new VirtualParallelRendering( volume , WIDTH , HEIGHT ,
                                                      VIRTUAL_GPUS );
#else
    parallelRenderer_ = new ParallelRendering( volume , WIDTH , HEIGHT);
#endif


#ifdef PROFILE_SINGLE_GPU

    parallelRenderer_->addRenderingNode( DEPLOY_GPU_INDEX );
    frameContainers_[ DEPLOY_GPU_INDEX ]->setEnabled( true );
    labels[ DEPLOY_GPU_INDEX ]->
            setText( QString( labels[ DEPLOY_GPU_INDEX ]->text() )
                     + QString( "(active)" ));

#else
    int upTo =  min( frameContainers_.size() ,
                     parallelRenderer_->getMachineGPUsCount() );

    for( auto i = 0 ; i < upTo  ; i++ )
    {
        LOG_DEBUG( "Deploy GPU#%d" , i );
        parallelRenderer_->addRenderingNode( i );
        frameContainers_[ i ]->setEnabled( true );
        labels[ i ]->setText( QString( labels[ i ]->text( ))
                              + QString( "(active)" ));
    }
#endif

    LOG_DEBUG( "Distribute Volume" );
    parallelRenderer_->distributeBaseVolume1D();

    LOG_DEBUG("Add Compositing Node");
    //The Compositing GPU will be the RenderingGPU with index 0 .
    parallelRenderer_->addCompositingNode( 0 );

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

