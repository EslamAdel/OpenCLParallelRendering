#include "RenderingWindow_Gui.h"
#include "ui_RenderingWindow_Gui.h"

#include <QFileDialog>
#include <QDateTime>
#include <QPicture>
#include "QLabelMouseEvents.h"
#include "Logger.h"
#include "QtMath"
RenderingWindow_Gui::RenderingWindow_Gui(
        clparen::Parallel::CLAbstractParallelRenderer *parallelRenderer ,
        QWidget *parent )
    : QMainWindow( parent ),
      ui( new Ui::RenderingWindow_Gui )
{
    parallelRenderer_ = parallelRenderer ;
    frameworkReady_ = false ;

    // UI setup
    ui->setupUi( this );



    LOG_DEBUG( "Creating Parallel Rendering Framework!..." );

    QVector< QLabel* > labels ;

    frameContainers_.push_back( ui->frameContainer0 );
    labels.push_back( ui->labelGPU0 );

    frameContainers_.push_back( ui->frameContainer1 );
    labels.push_back( ui->labelGPU1 );

    frameContainers_.push_back( ui->frameContainer2 );
    labels.push_back( ui->labelGPU2 );

    connect( ui->initializeButton , SIGNAL(released( )) ,
             this , SLOT( initializeFramework_SLOT( )));


    ui->availableDevices->surveyGPUs();
    ui->availableDevices->setFixed( true );

    ui->renderingDevices->setFixed( false );
    ui->compositingDevice->setFixed( false );
    ui->compositingDevice->setMaxGPUs( 1 );
    ui->offsetSlider->setEnabled(false);
    ui->scaleSlider->setEnabled(false);
    ui->isoValueSlider->setEnabled(false);
    ui->xrayButton->setChecked(true);

    // disable active kernel radiobuttons when framework is not ready
    ui->xrayButton->setEnabled(false);
    ui->maxIntensityProjectionButton->setEnabled(false);
    ui->minIntensityProjectionButton->setEnabled(false);
    ui->averageIntensityProjectionButton->setEnabled(false);
    ui->isoSurfaceButton->setEnabled(false);
    //disable transformations and transfer function when framework is not ready.
    ui->tabWidget->setTabEnabled( TransformationTabIndex , false );
    ui->tabWidget->setTabEnabled( TransferFunctionTabIndex , false );

}

RenderingWindow_Gui::~RenderingWindow_Gui( )
{
    delete ui;
}

void RenderingWindow_Gui::intializeConnections_()
{

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
             this , SLOT( newXScaling_SLOT( int )));

    connect( ui->yScalingSlider , SIGNAL( valueChanged( int )),
             this , SLOT( newYScaling_SLOT( int )));

    connect( ui->zScalingSlider , SIGNAL( valueChanged( int )),
             this , SLOT( newZScaling_SLOT( int )));

    connect( ui->xyzScalingSlider , SIGNAL( valueChanged( int )),
             this , SLOT( newXYZScaling_SLOT( int )));

    connect( ui->brightnessSlider , SIGNAL( valueChanged( int )),
             this , SLOT( newBrightness_SLOT( int )));

    connect( ui->densitySlider , SIGNAL( valueChanged( int )),
             this , SLOT( newDensity_SLOT( int )));

    connect( ui->isoValueSlider , SIGNAL( valueChanged( int )),
             this , SLOT( newIsoValue_SLOT( int )));


    //capture button
    connect( ui->captureButton , SIGNAL( released( )) ,
             this , SLOT( captureView_SLOT( )));

    // trasnfer function
    //    connect( ui->scaleSlider , SIGNAL( valueChanged(int)) ,
    //             this , SLOT(newTransferFunctionScale_SLOT(int)) );


    //    connect( ui->offsetSlider , SIGNAL( valueChanged(int)) ,
    //             this , SLOT(newTransferFunctionOffset_SLOT(int)) );

    //    connect(ui->transferFunctionCheckBox ,SIGNAL(stateChanged(int)),
    //            parallelRenderer_,SLOT(tranferFunctionFlag_SLOT(int)));

    //    connect(ui->transferFunctionCheckBox ,SIGNAL(stateChanged(int)),
    //            this , SLOT(tFunctionSLiderControl_SLOT(int)));

    //parallelRenderer_
    connect( parallelRenderer_ ,
             SIGNAL( frameReady_SIGNAL(
                         QPixmap * ,
                         const Renderer::CLAbstractRenderer* )),
             this ,
             SLOT( frameReady_SLOT(
                       QPixmap * ,
                       const Renderer::CLAbstractRenderer* )));


    connect( parallelRenderer_ , SIGNAL( finalFrameReady_SIGNAL( QPixmap* )) ,
             this , SLOT( finalFrameReady_SLOT( QPixmap* )));

    // Rendering Types
    connect( ui->xrayButton, SIGNAL( toggled( bool )),
             this, SLOT( switchRenderingKernel_SLOT( )));

    connect( ui->maxIntensityProjectionButton ,
             SIGNAL( toggled( bool )) ,
             this, SLOT( switchRenderingKernel_SLOT( )));

    connect( ui->minIntensityProjectionButton ,
             SIGNAL( toggled( bool )) ,
             this , SLOT( switchRenderingKernel_SLOT( )));

    connect( ui->averageIntensityProjectionButton ,
             SIGNAL( toggled( bool )) ,
             this , SLOT( switchRenderingKernel_SLOT( )));

    connect( ui->isoSurfaceButton ,
             SIGNAL( toggled( bool )) ,
             this , SLOT( switchRenderingKernel_SLOT( )));

    connect (ui->frameContainerResult,
             SIGNAL( mousePressed( QVector2D )),
             this , SLOT (mousePressed_SLOT( QVector2D )));

    connect (ui->frameContainerResult ,
             SIGNAL( mouseMoved( QVector2D )),
             this , SLOT ( mouseMoved_SLOT(QVector2D)) );

    connect (ui->frameContainerResult ,
             SIGNAL( mouseReleased(QVector2D) ),
             this , SLOT ( mouseReleased_SLOT(QVector2D) ));


}


void RenderingWindow_Gui::startRendering_( )
{
    if( ! frameworkReady_ )
        LOG_ERROR("Framework not intialized!");

    // Getting the initial values from the sliders
    newXTranslation_SLOT( ui->xTranslationSlider->value( ));
    newYTranslation_SLOT( ui->yTranslationSlider->value( ));
    newZTranslation_SLOT( ui->zTranslationSlider->value( ));
    newXRotation_SLOT( ui->xRotationSlider->value( ));
    newYRotation_SLOT( ui->yRotationSlider->value( ));
    newZRotation_SLOT( ui->zRotationSlider->value( ));
    newXScaling_SLOT( ui->xScalingSlider->value( ) );
    newYScaling_SLOT( ui->yScalingSlider->value( ));
    newZScaling_SLOT( ui->zScalingSlider->value( ) );
    newXYZScaling_SLOT( ui->xyzScalingSlider->value( ) );
    newBrightness_SLOT( ui->brightnessSlider->value( ));
    newDensity_SLOT( ui->densitySlider->value( ));

    parallelRenderer_->startRendering( );

}

void RenderingWindow_Gui::displayFrame_( QPixmap *frame , uint id )
{
//    LOG_DEBUG("Set Pixmap<%d>",id );
    if( frame->isNull() )
        LOG_WARNING("Null frame");

    frameContainers_[ id ]->setPixmap
            (( frame->scaled( frameContainers_[ id ]->width( ),
                              frameContainers_[ id ]->height( ),
                              Qt::KeepAspectRatio )));


}

void RenderingWindow_Gui::frameReady_SLOT( QPixmap *frame,
                                           const Renderer::CLAbstractRenderer *renderer )
{
    uint index = renderer->getFrameIndex();

//    LOG_DEBUG("Catch frame <%d> ", renderer->getFrameIndex());

    if( index < frameContainers_.size() )
        displayFrame_( frame , index );
}

void RenderingWindow_Gui::finalFrameReady_SLOT( QPixmap *finalFrame )
{
//    LOG_DEBUG("Set Final Frame" );

    finalFrame_ = finalFrame;
    ui->frameContainerResult->
            setPixmap( finalFrame->scaled( ui->frameContainerResult->width( ) ,
                                           ui->frameContainerResult->height( ) ,
                                           Qt::KeepAspectRatio ));
}

void RenderingWindow_Gui::newXRotation_SLOT(int value)
{
    ui->xRotationValue->setText(QString::number(value));

    parallelRenderer_->updateRotationX_SLOT( value );
}

void RenderingWindow_Gui::newYRotation_SLOT(int value)
{
    ui->yRotationValue->setText(QString::number(value));

    parallelRenderer_->updateRotationY_SLOT( value );
}

void RenderingWindow_Gui::newZRotation_SLOT(int value)
{
    ui->zRotationValue->setText( QString::number( value ));

    parallelRenderer_->updateRotationZ_SLOT( value );

}

void RenderingWindow_Gui::newXTranslation_SLOT(int value)
{
    ui->xTranslationValue->setText( QString::number( value ));

    parallelRenderer_->updateTranslationX_SLOT( value );
}

void RenderingWindow_Gui::newYTranslation_SLOT(int value)
{
    ui->yTranslationValue->setText( QString::number( value ));

    parallelRenderer_->updateTranslationY_SLOT( value );
}

void RenderingWindow_Gui::newZTranslation_SLOT(int value)
{
    ui->zTranslationValue->setText( QString::number( value ));
    parallelRenderer_->updateTranslationZ_SLOT( value );

}

void RenderingWindow_Gui::newXScaling_SLOT(int value)
{
    ui->xScalingValue->setText( QString::number( value ));
    parallelRenderer_->updateScaleX_SLOT( value );
}

void RenderingWindow_Gui::newYScaling_SLOT(int value)
{
    ui->yScalingValue->setText( QString::number( value ));
    parallelRenderer_->updateScaleY_SLOT( value );
}

void RenderingWindow_Gui::newZScaling_SLOT(int value)
{
    ui->zScalingValue->setText( QString::number( value ));
    parallelRenderer_->updateScaleZ_SLOT( value );
}

void RenderingWindow_Gui::newXYZScaling_SLOT(int value)
{
    ui->xyzScalingValue->setText( QString::number( value ));
    parallelRenderer_->updateScaleXYZ_SLOT( value );
}


void RenderingWindow_Gui::newBrightness_SLOT(int value)
{
    ui->brightnessValue->setText( QString::number( value ));

    float brightness =  float( value ) / 100.0;
    parallelRenderer_->updateImageBrightness_SLOT( brightness );
}

void RenderingWindow_Gui::newDensity_SLOT(int value)
{
    ui->densityValue->setText( QString::number( value ));
    float density = float( value ) / 100.0;
    parallelRenderer_->updateVolumeDensity_SLOT( density );
}

void RenderingWindow_Gui::newIsoValue_SLOT(int value)
{
    float isoValue = value / 10.0;
    ui->isoValue->setText( QString::number( isoValue ));
    parallelRenderer_->updateIsoValue_SLOT( isoValue );

}



//void RenderingWindow_Gui::newTransferFunctionScale_SLOT(int value)
//{
//    ui->scaleValue->setText( QString::number(1.0+(float)value/100 ));

//    float scale = 1.0f+float( value ) / 100.0;
//    parallelRenderer_->updateTransferFunctionScale_SLOT( scale );

//}

//void RenderingWindow_Gui::newTransferFunctionOffset_SLOT(int value)
//{
//    ui->offsetValue->setText( QString::number((float) value/100));

//    float offset = float( value ) / 100.0;
//    parallelRenderer_->updateTransferFunctionOffset_SLOT( offset );

//}

//void RenderingWindow_Gui::tFunctionSLiderControl_SLOT(int state)
//{
//    if(state == 0)
//    {
//        ui->offsetSlider->setEnabled(false);
//        ui->scaleSlider->setEnabled(false);
//    }
//    else
//    {
//        ui->offsetSlider->setEnabled(true);
//        ui->scaleSlider->setEnabled(true);

//    }

//}

void RenderingWindow_Gui::initializeFramework_SLOT()
{

    const QList< uint > &compositors =
            ui->compositingDevice->getGPUsIndices().toList();

    const QSet< uint > &renderers = ui->renderingDevices->getGPUsIndices();

    deployGPUs_ = renderers.toList().toVector();

    if( compositors.size() != 1 || renderers.isEmpty() )
    {
        LOG_WARNING("At least one GPU should be selected for "
                    "rendering and compositing!");
        return ;
    }

    connect( parallelRenderer_ , SIGNAL( frameworkReady_SIGNAL( )) ,
             this , SLOT( frameworkReady_SLOT( )));


    TaskInitializeFramework *task =
            new TaskInitializeFramework( parallelRenderer_ ,
                                         renderers ,
                                         compositors.at( 0 ));



    QThreadPool::globalInstance()->start( task );

    ui->initializeButton->setText( QString("Initializing..."));
    ui->initializeButton->setEnabled( false );


}

void RenderingWindow_Gui::frameworkReady_SLOT()
{
    frameworkReady_ = true ;

    ui->initializeButton->setText( QString("Framework ready!"));
    //disable transformations and transfer function when framework is not ready.
    ui->tabWidget->setTabEnabled( TransformationTabIndex , true );
    ui->tabWidget->setTabEnabled( TransferFunctionTabIndex , true );

    ui->xrayButton->setEnabled(true);
    ui->maxIntensityProjectionButton->setEnabled(true);
    ui->minIntensityProjectionButton->setEnabled(true);
    ui->averageIntensityProjectionButton->setEnabled(true);
    ui->isoSurfaceButton->setEnabled(true);

    intializeConnections_();
    startRendering_();
}

void RenderingWindow_Gui::switchRenderingKernel_SLOT()
{
    if( ui->xrayButton->isChecked( ))
        parallelRenderer_->
                activateRenderingKernel_SLOT(
                    CLKernel::RenderingMode::RENDERING_MODE_Xray );

    else if( ui->maxIntensityProjectionButton->isChecked( ))
        parallelRenderer_->
                activateRenderingKernel_SLOT(
                    CLKernel::RenderingMode::RENDERING_MODE_MaxIntensity );

    else if(ui->minIntensityProjectionButton->isChecked( ))
        parallelRenderer_->
                activateRenderingKernel_SLOT(
                    CLKernel::RenderingMode::RENDERING_MODE_MinIntensity );

    else if(ui->averageIntensityProjectionButton->isChecked( ))
        parallelRenderer_->
                activateRenderingKernel_SLOT(
                    CLKernel::RenderingMode::RENDERING_MODE_AverageIntensity );

    else if(ui->isoSurfaceButton->isChecked( ))
        parallelRenderer_->
                activateRenderingKernel_SLOT(
                    CLKernel::RenderingMode::RENDERING_MODE_IsoSurface );


    ui->isoValueSlider->setEnabled( ui->isoSurfaceButton->isChecked( ));
}

void RenderingWindow_Gui::mousePressed_SLOT(QVector2D mousePressedPosition)
{

    LOG_DEBUG("Mouse pressed!" );
    lastMousePosition_ =  mousePressedPosition;

}

void RenderingWindow_Gui::mouseMoved_SLOT(QVector2D newMousePosition)
{

    LOG_DEBUG("Mouse moved");

    mousePositionDifference_ = newMousePosition - lastMousePosition_;

    // Rotation axis is perpendicular to the mouse position difference
    // update x rotation angle
    mouseXRotationAngle_ = ui->xRotationSlider->value();
    mouseXRotationAngle_ += mousePositionDifference_.y();
    // update y rotation angle
    mouseYRotationAngle_ = ui->yRotationSlider->value();
    mouseYRotationAngle_ += mousePositionDifference_.x();

    LOG_DEBUG("y diff :%f", mousePositionDifference_.y() );
    LOG_DEBUG("x diff :%f", mousePositionDifference_.x() );
    LOG_DEBUG("Rot x :%f",  mouseXRotationAngle_ );
    LOG_DEBUG("Rot y :%f",  mouseYRotationAngle_ );

    if(mouseXRotationAngle_ >= 360)
        mouseXRotationAngle_ = 0;

    newMouseXRotation( );

    if(mouseYRotationAngle_ >= 360)
        mouseYRotationAngle_ = 0;

    newMouseYRotation( );

    // update last position
    lastMousePosition_ = newMousePosition;

}

void RenderingWindow_Gui::mouseReleased_SLOT(QVector2D releasedPosition)
{
    LOG_DEBUG("Mouse released");
}

void RenderingWindow_Gui::newMouseXRotation( )
{
    ui->xRotationSlider->setValue( mouseXRotationAngle_);
    parallelRenderer_->updateRotationX_SLOT( mouseXRotationAngle_ );
}


void RenderingWindow_Gui::newMouseYRotation( )
{
    ui->yRotationSlider->setValue( mouseYRotationAngle_);
    parallelRenderer_->updateRotationY_SLOT( mouseYRotationAngle_ );

}

void RenderingWindow_Gui::captureView_SLOT()
{
    QString dir =
            QFileDialog::
            getExistingDirectory( this , tr( "Open Directory" ) ,
                                  "/home" ,
                                  QFileDialog::ShowDirsOnly
                                  | QFileDialog::DontResolveSymlinks);


    QString date = QDateTime::currentDateTime().toString( "hh-mm-ss" );

    const uint width = parallelRenderer_->getFrameWidth();
    const uint height = parallelRenderer_->getFrameHeight();

    QString newDir = dir + QString( "/" ) + date +
            QString( "[%1x%2]" ).arg( QString::number( width ) ,
                                      QString::number( height ));
    QDir createDir;
    createDir.mkdir( newDir );
    LOG_DEBUG("New Dir:%s" , newDir.toStdString().c_str() );


    LOG_DEBUG("Saving resultant frame");
    QPixmap pic( parallelRenderer_->getCLCompositor().getFinalFrame().
                 value< clparen::CLData::CLFrame< float > *>()->
                 getFramePixmap().
                 scaledToHeight( width ).scaledToWidth( height ));
    pic.save( newDir + "/result.jpg");



    for( const uint gpuIndex : deployGPUs_ )
    {

        LOG_DEBUG("Saving frame<%d>", gpuIndex );
        CLData::CLFrame< float > *sourceFrame =
                parallelRenderer_->getCLRenderer( gpuIndex ).
                getCLFrame().value< CLData::CLFrame< float > *>() ;

        QPixmap framePixmap( sourceFrame->
                             getFramePixmap().
                             scaledToHeight( width ).
                             scaledToWidth( height ));

        framePixmap.save( newDir + QString("/GPU%1.jpg").arg( gpuIndex ));

    }
}
