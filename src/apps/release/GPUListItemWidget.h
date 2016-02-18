#ifndef GPULISTITEMWIDGET_H
#define GPULISTITEMWIDGET_H
#include <QIcon>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>

class GPUListItemWidget : public QWidget
{
public:
    GPUListItemWidget( QPixmap picture , QString info , uint index ,
                       QWidget *parent = 0 );

    QHBoxLayout *getLayout( ) ;

    const QPixmap &getPicture( ) const ;

    const QPixmap &getOnFlyPicture( ) const ;

    void setPicture( const QPixmap picture ) ;

    const QString &getInfo( ) const ;


    const uint getGPUIndex() const ;



private:
    QHBoxLayout *layout_ ;

    QLabel *gpuIcon_ ;
    QLabel *label_ ;

    QPixmap picture_ ;
    QPixmap onFlyPicture_ ;

    const uint gpuIndex_ ;
    const QString info_ ;
};

#endif // GPULISTITEMWIDGET_H
