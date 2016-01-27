#include "RenderingWindow.h"
#include "ui_RenderingWindow.h"
#include <Logger.h>

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

    LOG_DEBUG( "Deploy GPU#0" );
    parallelRenderer_->addRenderingNode( 0 );

    LOG_DEBUG( "Deploy GPU#1" );
    parallelRenderer_->addRenderingNode( 1 );

    LOG_DEBUG( "Distribute Volume" );
    parallelRenderer_->distributeBaseVolume1D();


    connect( parallelRenderer_ , SIGNAL( framesReady_SIGNAL( )),
            this , SLOT( framesReady_SLOT( )));

    LOG_INFO( "Triggering rendering" );
    startRendering_( );
}

RenderingWindow::~RenderingWindow( )
{
    delete ui;
}

void RenderingWindow::startRendering_( )
{
    // Getting the initial values from the sliders
    on_xSlider_valueChanged( ui->xSlider->value( ));
    on_ySlider_valueChanged( ui->ySlider->value( ));
    on_zSlider_valueChanged( ui->zSlider->value( ));

    on_xTranslationSlider_valueChanged( ui->xTranslationSlider->value( ));
    on_yTranslationSlider_valueChanged( ui->yTranslationSlider->value( ));

    on_brightnessSlider_valueChanged( ui->brightnessSlider->value( ));
    on_densitySlider_valueChanged( ui->densitySlider->value( ));

    parallelRenderer_->startRendering( );

}

void RenderingWindow::displayFrame_( )
{

    //LOG_DEBUG("displaying frames");
    //LOG_DEBUG("rendering node ptr:%")
    QPixmap frame0 = *( parallelRenderer_->getRenderingNode( 0 )
                        .getContext()->getFrame() );
    QPixmap frame1 = *( parallelRenderer_->getRenderingNode( 1 )
                        .getContext()->getFrame() );

    ui->frameContainer0->setPixmap
            (( frame0.scaled( ui->frameContainer0->width( ),
                              ui->frameContainer0->height( ),
                              Qt::KeepAspectRatio )));
    ui->frameContainer1->setPixmap
            (( frame1.scaled( ui->frameContainer1->width( ),
                              ui->frameContainer1->height( ),
                              Qt::KeepAspectRatio )));
}

void RenderingWindow::framesReady_SLOT()
{
    displayFrame_();
}

void RenderingWindow::on_xSlider_valueChanged( int value )
{
    ui->xRotationLabel->setText( QString::number( value ));

    parallelRenderer_->updateRotationX_SLOT( value );

}

void RenderingWindow::on_ySlider_valueChanged( int value )
{
    ui->yRotationLabel->setText( QString::number( value ));

    parallelRenderer_->updateRotationY_SLOT( value );

}

void RenderingWindow::on_zSlider_valueChanged( int value )
{
    ui->zRotationLabel->setText( QString::number( value ));

    parallelRenderer_->updateRotationZ_SLOT( value );

}

void RenderingWindow::on_xTranslationSlider_valueChanged(int value)
{
    ui->xTranslationLabel->setText( QString::number( value ));

    parallelRenderer_->updateTranslationX_SLOT( value );

}

void RenderingWindow::on_yTranslationSlider_valueChanged(int value)
{
    ui->yTranslationLabel->setText( QString::number( value ));

    parallelRenderer_->updateTranslationY_SLOT( value );

}

void RenderingWindow::on_brightnessSlider_valueChanged( int value )
{
    ui->brightnessLabel->setText( QString::number( value ));

    float brightness =  float( value ) / 100.0;
    parallelRenderer_->updateImageBrightness_SLOT( brightness );

}

void RenderingWindow::on_densitySlider_valueChanged( int value )
{
    ui->densityLabel->setText( QString::number( value ));

    float density = float( value ) / 100.0;
    parallelRenderer_->updateVolumeDensity_SLOT( density );

}
