#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Logger.h"
#include "HoverPoints.h"
#include "Utilities.h"

#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <QTreeWidget>



class ColorMapWidget;

MainWindow::MainWindow(
        clparen::Parallel::CLAbstractParallelRenderer &parallelRenderer ,
        QWidget *parent
        ) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
  , _redWidget( new ColorMapWidget( ColorMapWidget::RED_SHADE, this ))
  , _greenWidget( new ColorMapWidget( ColorMapWidget::GREEN_SHADE, this ))
  , _blueWidget( new ColorMapWidget( ColorMapWidget::BLUE_SHADE, this ))
  , _alphaWidget( new ColorMapWidget( ColorMapWidget::ARGB_SHADE, this ))
  , parallelRenderer_( parallelRenderer )
{
    // RGBA * 256
    transferFunctionTable_ = new float[1024];

    qRegisterMetaType< UInt8Vector >("UInt8Vector");
    ui->setupUi(this);

    ui->redLayout->addWidget( _redWidget );
    ui->greenLayout->addWidget( _greenWidget );
    ui->blueLayout->addWidget( _blueWidget );
    ui->rgbaLayout->addWidget( _alphaWidget );

    connect( _redWidget, SIGNAL( colorsChanged( )), this,
             SLOT( _pointsUpdated()));
    connect( _greenWidget, SIGNAL( colorsChanged( )), this,
             SLOT( _pointsUpdated( )));
    connect( _blueWidget, SIGNAL( colorsChanged( )), this,
             SLOT( _pointsUpdated( )));
    connect( _alphaWidget, SIGNAL( colorsChanged( )), this,
             SLOT( _pointsUpdated( )));

    connect( this, &MainWindow::transferFunctionChanged,
             this, &MainWindow::_onTransferFunctionChanged );

    connect( ui->resetCMButton, SIGNAL( clicked()), this, SLOT( _setDefault()));
    connect( ui->resetCMButton, SIGNAL( clicked()), this, SLOT( _clear()));
    connect( ui->cmLoadButton, SIGNAL( clicked()), this, SLOT( _load()));
    connect( ui->cmSaveButton, SIGNAL( clicked()), this, SLOT( _save()));

    QTimer::singleShot( 50, this, SLOT( _setDefault()));


    LOG_INFO( "Triggering rendering" );
    startRendering_( );


    // Getting the initial values from the sliders
    on_xRotationSlider_valueChanged( ui->xRotationSlider->value( ));
    on_yRotationSlider_valueChanged( ui->yRotationSlider->value( ));
    on_zRotationSlider_valueChanged( ui->zRotationSlider->value( ));

    on_xTranslationSlider_valueChanged( ui->xTranslationSlider->value( ));
    on_yTranslationSlider_valueChanged( ui->yTranslationSlider->value( ));

    on_brightnessSlider_2_valueChanged( ui->brightnessSlider_2->value( ));
    on_densitySlider_valueChanged( ui->densitySlider->value( ));

    on_maximumStepsSlider_valueChanged( ui->maximumStepsSlider->value( ));
    on_stepSizeSlider_valueChanged( ui->stepSizeSlider->value( ));

}


void MainWindow::_pointsUpdated()
{
    const double alphaWidgetWidth = _alphaWidget->width();
    QGradientStops stops;
    QPolygonF redPoints, greenPoints, bluePoints, alphaPoints, allPoints;

    redPoints = _redWidget->getPoints();
    greenPoints = _greenWidget->getPoints();
    bluePoints = _blueWidget->getPoints();
    alphaPoints = _alphaWidget->getPoints();

    std::sort( redPoints.begin(), redPoints.end(), xLessThan );
    std::sort( greenPoints.begin(), greenPoints.end(), xLessThan );
    std::sort( bluePoints.begin(), bluePoints.end(), xLessThan );
    std::sort( alphaPoints.begin(), alphaPoints.end(), xLessThan );

    allPoints = redPoints;
    allPoints += greenPoints;
    allPoints += bluePoints;
    allPoints += alphaPoints;
    std::sort( allPoints.begin(), allPoints.end(), xLessThan );

    for( int32_t i = 0; i < allPoints.size(); ++i )
    {
        const int xPoint = int( allPoints.at(i).x());
        if( i + 1 < allPoints.size() && xPoint == allPoints.at(i + 1).x( ))
            continue;

        const int red = _redWidget->getColorAtPoint( xPoint );
        const int green = _greenWidget->getColorAtPoint( xPoint );
        const int blue = _blueWidget->getColorAtPoint( xPoint );
        const int alpha = _alphaWidget->getColorAtPoint( xPoint );
        QColor color((0x00ff0000 & red) >> 16,      // R (16)
                     (0x0000ff00 & green) >> 8,     // G (8)
                     (0x000000ff & blue),           // B (1)
                     (0xff000000 & alpha) >> 24);   // A (24)

        // Outlier
        if( xPoint / alphaWidgetWidth > 1 )
            return;

        stops << QGradientStop( xPoint / alphaWidgetWidth, color );
    }

    _publishTransferFunction();
    _alphaWidget->setGradientStops( stops );
}

void MainWindow::_setDefault()
{
    QPolygonF points;
    double h = _redWidget->height();
    double w = _redWidget->width();
    points << QPointF( 0.0 * w, h );
    points << QPointF( 0.4 * w, h );
    points << QPointF( 0.6 * w, 0.0 );
    points << QPointF( 1.0 * w, 0.0 );
    _redWidget->setPoints( points );

    h = _greenWidget->height();
    w = _greenWidget->width();
    points.clear();
    points << QPointF( 0.0 * w, h );
    points << QPointF( 0.2 * w, 0.0 );
    points << QPointF( 0.6 * w, 0.0 );
    points << QPointF( 0.8 * w, h );
    points << QPointF( 1.0 * w, h );
    _greenWidget->setPoints( points );

    h = _blueWidget->height();
    w = _blueWidget->width();
    points.clear();
    points << QPointF( 0.0 * w, 0.0 );
    points << QPointF( 0.2 * w, 0.0 );
    points << QPointF( 0.4 * w, h );
    points << QPointF( 0.8 * w, h );
    points << QPointF( 1.0 * w, 0.0 );
    _blueWidget->setPoints( points );

    h = _alphaWidget->height();
    w = _alphaWidget->width();
    points.clear();
    points << QPointF( 0.0 * w, h );
    points << QPointF( 0.1 * w, 0.8 * h );
    points << QPointF( 1.0 * w, 0.1 * h );
    _alphaWidget->setPoints( points );

    _pointsUpdated();
}

void MainWindow::_publishTransferFunction()
{
    const UInt8Vector& redCurve =  _redWidget->getCurve();
    const UInt8Vector& greenCurve =  _greenWidget->getCurve();
    const UInt8Vector& blueCurve =  _blueWidget->getCurve();
    const UInt8Vector& alphaCurve =  _alphaWidget->getCurve();

    if( redCurve.empty() || greenCurve.empty() || blueCurve.empty() ||
            alphaCurve.empty( ))
    {
        return;
    }

    for( uint32_t i = 0; i < redCurve.size(); ++i )
    {
        transferFunctionTable_[i * 4 + 0] = redCurve[i] / float( 256 );
        transferFunctionTable_[i * 4 + 1] = greenCurve[i] / float( 256 );
        transferFunctionTable_[i * 4 + 2] = blueCurve[i] / float( 256 );
        transferFunctionTable_[i * 4 + 3] = alphaCurve[i] / float( 256 );
    }

    // updateKernel
    parallelRenderer_.updateTransferFunction_SLOT( transferFunctionTable_ );

}

void MainWindow::_clear()
{
    QPolygonF points;
    const double h = _redWidget->height();
    const double w = _redWidget->width();
    points << QPointF( 0.0 * w, h );
    points << QPointF( 1.0 * w, 0.0 );

    _redWidget->setPoints( points );
    _greenWidget->setPoints( points );
    _blueWidget->setPoints( points );
    _alphaWidget->setPoints( points );
    _pointsUpdated();
}

const quint32 TF_FILE_HEADER = 0xdeadbeef;
const quint32 TF_FILE_VERSION = 1;
const QString TF_FILE_FILTER( "Transfer function files (*.tf)" );

void MainWindow::_load()
{
    const QString filename = QFileDialog::getOpenFileName( this, "Load transfer function",
                                                           QString(),
                                                           TF_FILE_FILTER );
    if( filename.isEmpty( ))
        return;

    QFile file( filename );
    file.open( QIODevice::ReadOnly );
    QDataStream in( &file );

    quint32 header;
    in >> header;
    if( header != TF_FILE_HEADER )
        return;

    quint32 version;
    in >> version;
    if( version != TF_FILE_VERSION )
        return;

    QPolygonF redPoints, greenPoints, bluePoints, alphaPoints;
    QGradientStops gradientStops;
    in >> redPoints
            >> greenPoints
            >> bluePoints
            >> alphaPoints;

    _redWidget->setPoints( redPoints );
    _greenWidget->setPoints( greenPoints );
    _blueWidget->setPoints( bluePoints );
    _alphaWidget->setPoints( alphaPoints );
    _pointsUpdated();
}

void MainWindow::_save()
{
    QString filename = QFileDialog::getSaveFileName( this, "Save transfer function",
                                                     QString(),
                                                     TF_FILE_FILTER );
    if( filename.isEmpty( ))
        return;

    if( !filename.endsWith( ".tf" ))
        filename.append( ".tf" );

    QFile file( filename );
    file.open( QIODevice::WriteOnly );
    QDataStream out( &file );
    out.setVersion( QDataStream::Qt_5_0 );

    out << TF_FILE_HEADER << TF_FILE_VERSION;
    out << _redWidget->getPoints()
        << _greenWidget->getPoints()
        << _blueWidget->getPoints()
        << _alphaWidget->getPoints();
}

namespace
{
QPolygon _filterPoints( const QPolygon& points )
{
    QPolygon filteredPoints;
    float prevSlope = 0;
    QPoint prevPoint = points.first();
    for( int i = 1; i < points.size() - 1; ++i )
    {
        const QPoint& currentPoint = points[i];
        const QLine currentLine( prevPoint, currentPoint );
        const float currentSlope = float(currentLine.dy()) / float(currentLine.dx());

        bool change = std::abs(prevSlope - currentSlope) > std::numeric_limits<float>::epsilon();
        if( change )
        {
            const QLine nextLine( currentPoint, points[i+1] );
            const float nextSlope = float(nextLine.dy()) / float(nextLine.dx());
            if( std::abs(prevSlope - nextSlope) <= std::numeric_limits<float>::epsilon() )
                change = false;
        }

        if( change || i == 1 )
        {
            prevSlope = currentSlope;
            filteredPoints << prevPoint;
        }
        prevPoint = points[i];
    }
    filteredPoints << points.last();
    return filteredPoints;
}

QPolygonF _convertPoints( const QPolygon& points, const int width, const int height )
{
    QPolygonF convertedPoints;
    for( int32_t i = 0; i < points.size(); ++i )
    {
        const int position = points.at( i ).x();
        const int value = points.at( i ).y();

        convertedPoints << QPointF( (position/255.f) * width,
                                    height - value * height / 255 );
    }
    return convertedPoints;
}
}

void MainWindow::_onTransferFunctionChanged( UInt8Vector tf )
{
    QGradientStops stops;
    QPolygon redPoints, bluePoints, greenPoints, alphaPoints;
    for( size_t i = 0; i < 256; ++i )
    {
        redPoints << QPoint(i, tf[i*4]);
        greenPoints << QPoint(i, tf[i*4+1]);
        bluePoints << QPoint(i, tf[i*4+2]);
        alphaPoints << QPoint(i, tf[i*4+3]);
        stops << QGradientStop( float(i) / 255.f , QColor( tf[i*4], tf[i*4+1], tf[i*4+2], tf[i*4+3]));
    }

    QPolygonF redPointsF = _convertPoints( _filterPoints( redPoints ),
                                           _redWidget->width(),
                                           _redWidget->height());
    QPolygonF greenPointsF = _convertPoints( _filterPoints( greenPoints ),
                                             _greenWidget->width(),
                                             _greenWidget->height());
    QPolygonF bluePointsF = _convertPoints( _filterPoints( bluePoints ),
                                            _blueWidget->width(),
                                            _blueWidget->height());
    QPolygonF alphaPointsF = _convertPoints( _filterPoints( alphaPoints ),
                                             _alphaWidget->width(),
                                             _alphaWidget->height());

    _redWidget->setPoints( redPointsF );
    _greenWidget->setPoints( greenPointsF );
    _blueWidget->setPoints( bluePointsF );
    _alphaWidget->setPoints( alphaPointsF );
    _pointsUpdated();
}


void MainWindow::startRendering_( )
{

    //parallelRenderer_
    connect( &parallelRenderer_ , SIGNAL( finalFrameReady_SIGNAL( QPixmap* )) ,
             this , SLOT( displayFrame( QPixmap* )));

    parallelRenderer_.startRendering();

}

void MainWindow::displayFrame( QPixmap* frame )
{

    ui->frameContainer->setPixmap
            (( frame->scaled( ui->frameContainer->width( ),
                             ui->frameContainer->height( ),
                             Qt::KeepAspectRatio )));
}




MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_xRotationSlider_valueChanged(int value)
{
    ui->xRotationText->setText( QString::number( value ));

    parallelRenderer_.updateRotationX_SLOT( value );

}

void MainWindow::on_yRotationSlider_valueChanged(int value)
{
    ui->yRotationText->setText( QString::number( value ));

    parallelRenderer_.updateRotationY_SLOT( value );

}

void MainWindow::on_zRotationSlider_valueChanged(int value)
{
    ui->zRotationText->setText( QString::number( value ));

    parallelRenderer_.updateRotationZ_SLOT( value );

}

void MainWindow::on_xTranslationSlider_valueChanged(int value)
{
    ui->xTranslationText->setText( QString::number( value ));

    parallelRenderer_.updateTranslationX_SLOT( value );

}

void MainWindow::on_yTranslationSlider_valueChanged(int value)
{
    ui->yTranslationText->setText( QString::number( value ));

    parallelRenderer_.updateTranslationY_SLOT( value );

}

void MainWindow::on_xyDisplayResolutionSlider_valueChanged(int value)
{

}

void MainWindow::on_xyFrameResolutionSlider_valueChanged(int value)
{

}

void MainWindow::on_brightnessSlider_2_valueChanged(int value)
{
    ui->brightnessText_2->setText( QString::number( value ));

    float brightness =  float( value ) / 100.0;
    parallelRenderer_.updateImageBrightness_SLOT( brightness );


}

void MainWindow::on_xyBlockDimensionSlider_valueChanged(int value)
{

}

void MainWindow::on_densitySlider_valueChanged(int value)
{
    ui->densityText->setText( QString::number( value ));

    float density = float( value ) / 100.0;
    parallelRenderer_.updateVolumeDensity_SLOT( density );

}

void MainWindow::on_maximumStepsSlider_valueChanged(int value)
{
    ui->maximumStepsText->setText( QString::number( value ));

    parallelRenderer_.updateMaxSteps_SLOT( value );

}

void MainWindow::on_stepSizeSlider_valueChanged(int value)
{
    float tStep = float( value ) * 0.001;
    parallelRenderer_.updateStepSize_SLOT( tStep );

}

void MainWindow::on_apexAngleSlider_valueChanged(int value)
{
    ui->apexAngleText->setText( QString::number( value ));

    parallelRenderer_.updateApexAngle_SLOT( float(value) );

}

void MainWindow::on_xyzScaleSlider_valueChanged(int value)
{

}
