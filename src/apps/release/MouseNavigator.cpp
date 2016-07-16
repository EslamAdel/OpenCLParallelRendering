#include "MouseNavigator.h"
#include "Logger.h"


MouseNavigator::MouseNavigator(QWidget *parent) :QLabel(parent)
{

}


void MouseNavigator::mousePressEvent(QMouseEvent *event)
{
    emit mousePressed(QVector2D(event->localPos()));
}

void MouseNavigator::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMoved(QVector2D(event->localPos()));
}



void MouseNavigator::wheelEvent(QWheelEvent *event)
{
    emit mouseWheelMoved(event);
}

