#include "QLabelMouseEvents.h"
#include "Logger.h"


QLabelMouseEvents::QLabelMouseEvents(QWidget *parent) :QLabel(parent)
{

}


void QLabelMouseEvents::mousePressEvent(QMouseEvent *event)
{
    emit mousePressed(QVector2D(event->localPos()));
}

void QLabelMouseEvents::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMoved(QVector2D(event->localPos()));
}


void QLabelMouseEvents::mouseReleaseEvent(QMouseEvent *event)
{
    emit mouseReleased(QVector2D(event->localPos()));
}

