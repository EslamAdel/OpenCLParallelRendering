#include "GPUListView.h"
#include <oclHWDL.h>
#include "Logger.h"

GPUListView::GPUListView( QWidget *&centeralWidget )
    : QListWidget( centeralWidget )
{
    setMinimumSize(100, 100);
    setAcceptDrops(true);

    limited_ = false ;

    QPixmap nvidiaIcon( "://nvidia" );
    QPixmap atiIcon( "://ati" );
    QPixmap intelIcon( "://intel" );
    QPixmap otherIcon( "://other" );

    if( nvidiaIcon.isNull() )
        LOG_ERROR("Null pixmap object");

    LOG_DEBUG("Icons");
    vendorsIcons_[ Vendor::Nvidia ] = nvidiaIcon.scaled( 50 , 50 , Qt::KeepAspectRatio ) ;
    vendorsIcons_[ Vendor::ATI ] = atiIcon.scaled( 50 , 50 , Qt::KeepAspectRatio ) ;
    vendorsIcons_[ Vendor::Intel ] = intelIcon.scaled( 50 , 50 , Qt::KeepAspectRatio ) ;
    vendorsIcons_[ Vendor::Other ] = otherIcon.scaled( 50 , 50 , Qt::KeepAspectRatio ) ;
}

void GPUListView::setFixed( bool fixed )
{
    fixed_ = fixed ;
}

void GPUListView::setMaxGPUs( uint maxGPUs )
{
    limited_ = true ;
    maxGPUs_ = maxGPUs ;
}

void GPUListView::addGPUItem( const GPUListView::Vendor vendor ,
                              const QString info ,
                              uint index  )
{
    if( gpuIndices_.contains( index ))
        return ;

    QListWidgetItem *item = new QListWidgetItem( this );

    GPUListItemWidget *gpu =
            new GPUListItemWidget( vendorsIcons_[ vendor ] ,
                                   info , index  );

    gpu->setAttribute(Qt::WA_DeleteOnClose);
    item->setSizeHint( gpu->getPicture().size() + QSize( 15 , 15 ) );

    this->addItem( item ) ;
    this->setItemWidget( item , gpu ) ;

    gpuIndices_ << index ;
    items_[ index ] = item ;

}

const QSet<uint> &GPUListView::getGPUsIndices() const
{
    return gpuIndices_ ;

}

void GPUListView::surveyGPUs()
{
    oclHWDL::Hardware *oclHardware = new oclHWDL::Hardware();
    std::vector< oclHWDL::Device* > devices =  oclHardware->getListGPUs();

    uint index = 0;
    for( oclHWDL::Device* device : devices )
    {
        std::string deviceVendor = device->getVendor();


        float ramMemory = device->getGlobalMemorySize() / ( 1024*1024*1024.f);
        std::string name = device->getName();

        GPUListView::Vendor vendor ;
        if( deviceVendor == std::string("NVIDIA Corporation"))
            vendor = GPUListView::Vendor::Nvidia;

        else if ( deviceVendor == std::string("Advanced Micro Devices, Inc."))
            vendor = GPUListView::Vendor::ATI ;

        else if( deviceVendor == std::string("Intel(R) Corporation")
                 || deviceVendor == std::string("GenuineIntel"))
            vendor = GPUListView::Vendor::Intel ;

        else
            vendor = GPUListView::Vendor::Other ;

        QString  info( QString( name.c_str( )) + QString::number( index )
                       + "\n"
                       + "Ram: " + QString::number( ramMemory )
                       + "(GB)");


        addGPUItem( vendor , info , index );
        index++;
    }
}


void GPUListView::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void GPUListView::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void GPUListView::dropEvent(QDropEvent *event)
{


    if( event->source() == this)
        event->ignore();


    if( limited_ )
    {
        if( this->count() >= maxGPUs_ )
            return;
    }

    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPixmap picture;
        QString info;
        uint gpuIndex;
        QPoint offset;

        dataStream
                >> info
                >> picture
                >> gpuIndex
                >> offset;


        if( gpuIndices_.contains( gpuIndex ))
            return ;

        QListWidgetItem *item = new QListWidgetItem( this );

        GPUListItemWidget *newGPU =
                new GPUListItemWidget( picture ,
                                       info , gpuIndex ,
                                       dynamic_cast< QWidget* >( item ) );

        item->setSizeHint( newGPU->getPicture().size() + QSize( 15 , 15 ) );


        this->addItem( item );
        this->setItemWidget( item , newGPU );
        newGPU->move(event->pos() - offset);
        newGPU->setAttribute(Qt::WA_DeleteOnClose);

        gpuIndices_ << gpuIndex ;
        items_[ gpuIndex ] = item ;

    }
    else
        event->ignore();


    qDebug()<<"dropped here!";
}

void GPUListView::mousePressEvent(QMouseEvent *event)
{

    if( this->count() == 0 )
        return;

    GPUListItemWidget *gpu =
            dynamic_cast< GPUListItemWidget* >(childAt(event->pos()));

    if( !gpu )
    {
        QLabel *label =
                dynamic_cast< QLabel* >(childAt(event->pos()));

        if( !label )
            return ;

        gpu = dynamic_cast< GPUListItemWidget* >( label->parent());
    }
    if( !gpu )
        return ;

    qDebug() << "I am pressed!" ;


    QByteArray itemData;
    QDataStream dataStream( &itemData , QIODevice::WriteOnly );

    dataStream << gpu->getInfo()
               << gpu->getPicture()
               << gpu->getGPUIndex()
               << QPoint(event->pos() - gpu->pos());

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-dnditemdata", itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap( gpu->getOnFlyPicture( ));
    drag->setHotSpot(event->pos() - gpu->pos());


    if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction)
    {            qDebug() << "Remove!";
        gpu->close();

    }
    else
        gpu->show( );

    if( !fixed_ )
    {
        qDebug() << "Remove!";
        uint index = gpu->getGPUIndex();
        gpuIndices_.remove( index );
        gpu->close();

        QListWidgetItem *item = items_[ index ];
        removeItemWidget( item );
        delete item;
    }

}

