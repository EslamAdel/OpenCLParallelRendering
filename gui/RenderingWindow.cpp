#include "RenderingWindow.h"
#include "ui_RenderingWindow.h"
#include <Logger.h>

#define MAX_NODES 3

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

    parallelRenderer_ = new ParallelRendering( volume );

    frameContainers_.push_back( ui->frameContainer0 );
    frameContainers_.push_back( ui->frameContainer1 );
    frameContainers_.push_back( ui->frameContainer2 );

    for( auto i = 0 ; i < parallelRenderer_->machineGPUsCount() ; i++ )
    {
        LOG_DEBUG( "Deploy GPU#%d" , i );
        parallelRenderer_->addRenderingNode( i );
    }

    LOG_DEBUG( "Distribute Volume" );
    parallelRenderer_->distributeBaseVolume1D();


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
    connect( parallelRenderer_ , SIGNAL( framesReady_SIGNAL( )),
            this , SLOT( framesReady_SLOT( )));


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

void RenderingWindow::displayFrame_( )
{

    //LOG_DEBUG("displaying frames");
    //LOG_DEBUG("rendering node ptr:%")

    for( auto i = 0 ; i < parallelRenderer_->activeRenderingNodesCount() ; i++)
    {
        QPixmap frame = *( parallelRenderer_->getRenderingNode( i )
                           .getContext()->getFrame() );

        frameContainers_[ i ]->setPixmap
                (( frame.scaled( frameContainers_[ i ]->width( ),
                                 frameContainers_[ i ]->height( ),
                                 Qt::KeepAspectRatio )));

    }

}

void RenderingWindow::framesReady_SLOT()
{
    displayFrame_();
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

