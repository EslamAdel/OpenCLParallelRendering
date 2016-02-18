#include "GPUListItemWidget.h"
#include <QPainter>
#include <QStyleOption>

GPUListItemWidget::GPUListItemWidget( QPixmap picture , QString info ,
                                      uint index ,
                                      QWidget *parent  )
    : QWidget( parent ) ,
      picture_( picture.scaled( 50 , 50 , Qt::KeepAspectRatio) ) ,
      info_( info ) ,
      gpuIndex_( index )
{

    layout_ = new QHBoxLayout( this );
    gpuIcon_ = new QLabel( this );
    label_ = new QLabel( this  );

    gpuIcon_->setPixmap( picture_ );
    label_->setText( info_ );

    layout_->addWidget( gpuIcon_ );
    layout_->addWidget( label_ );


    onFlyPicture_ = picture_  ;
    QPainter painter;
    painter.begin( &onFlyPicture_ );
    painter.fillRect( onFlyPicture_.rect() , QColor(127, 127, 127, 127));
    painter.end( );

}



QHBoxLayout *GPUListItemWidget::getLayout()
{
    return layout_;
}

const QPixmap &GPUListItemWidget::getPicture() const
{
    return picture_ ;
}

const QPixmap &GPUListItemWidget::getOnFlyPicture() const
{
    return onFlyPicture_ ;
}

void GPUListItemWidget::setPicture( const QPixmap picture )
{
    picture_ = picture ;
}


const QString &GPUListItemWidget::getInfo( ) const
{
    return info_ ;
}

const uint GPUListItemWidget::getGPUIndex() const
{
    return gpuIndex_ ;
}


