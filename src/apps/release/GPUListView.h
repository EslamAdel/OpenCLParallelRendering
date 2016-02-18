#ifndef GPULISTVIEW_H
#define GPULISTVIEW_H

#include "QListWidget"
#include "QDragEnterEvent"
#include "QMouseEvent"
#include "QMimeData"
#include "QDrag"
#include "QPainter"
#include "GPUListItemWidget.h"
#include <QDebug>
#include <unordered_map>


class GPUListView : public QListWidget
{
public:
    GPUListView( QWidget *&centeralWidget );

    void setFixed( bool fixed );

    void setMaxGPUs( uint maxGPUs );


enum Vendor : uint8_t
{
    Nvidia ,
    ATI ,
    Intel ,
    Other
};


public :
    void addGPUItem( const Vendor vendor , const QString info , uint index );


    const QSet< uint > &getGPUsIndices( ) const;


    void surveyGPUs();
protected:

    void dragEnterEvent( QDragEnterEvent *event ) Q_DECL_OVERRIDE;

    void dragMoveEvent( QDragMoveEvent *event ) Q_DECL_OVERRIDE;

    void dropEvent( QDropEvent *event ) Q_DECL_OVERRIDE;

    void mousePressEvent( QMouseEvent *event ) Q_DECL_OVERRIDE;


private:

    QSet< uint > gpuIndices_ ;

    QHash< const Vendor , QPixmap > vendorsIcons_ ;
    QHash<  const uint , QListWidgetItem* > items_ ;

    uint maxGPUs_ ;
    bool limited_ ;
    bool fixed_ ;
};

#endif // GPULISTVIEW_H
