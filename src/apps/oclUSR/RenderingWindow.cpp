#include "RenderingWindow.h"
#include "ui_RenderingWindow.h"
//#include <Logger.h>

/// TODO: Pass to arguments
#define VOLUME_PREFIX "/projects/volume-datasets/foot/foot"

RenderingWindow::RenderingWindow( QWidget *parent ) :
    QMainWindow( parent ),
    ui( new Ui::RenderingWindow )
{
//    // UI setup
//    ui->setupUi( this );

//    LOG_INFO( "Loading volume" );
//    const std::string prefix = VOLUME_PREFIX;
//    Volume< uchar >* volume = new Volume< uchar >( prefix );

//    LOG_DEBUG( "Creating OpenCL context w/o OpenGL at all !..." );
//    clContext_ = new CLContext< uint8_t >( volume, 0 );

//    LOG_INFO( "Triggering rendering" );
//    startRendering_( );
}

RenderingWindow::~RenderingWindow( )
{
    delete ui;
}

void RenderingWindow::startRendering_( )
{
//    // Getting the initial values from the sliders
//    on_xSlider_valueChanged( ui->xSlider->value( ));
//    on_ySlider_valueChanged( ui->ySlider->value( ));
//    on_zSlider_valueChanged( ui->zSlider->value( ));

//    on_xTranslationSlider_valueChanged( ui->xTranslationSlider->value( ));
//    on_yTranslationSlider_valueChanged( ui->yTranslationSlider->value( ));

//    on_brightnessSlider_valueChanged( ui->brightnessSlider->value( ));
//    on_densitySlider_valueChanged( ui->densitySlider->value( ));

//    clContext_->startRendering( );
//    displayFrame( );
}

void RenderingWindow::displayFrame( )
{
//    QPixmap frame = *( clContext_->getFrame( ));
//    ui->frameContainer->setPixmap
//            (( frame.scaled( ui->frameContainer->width( ),
//                             ui->frameContainer->height( ),
//                             Qt::KeepAspectRatio )));
}

