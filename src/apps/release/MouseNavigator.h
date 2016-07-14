#ifndef MOUSENAVIGATOR_H
#define MOUSENAVIGATOR_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QEvent>
#include <QWheelEvent>

class MouseNavigator : public QLabel
{
Q_OBJECT

public:

     explicit MouseNavigator(QWidget *parent = 0 );

     void mouseMoveEvent(QMouseEvent *event);

     void mousePressEvent(QMouseEvent *event);

     void mouseReleaseEvent(QMouseEvent *event);

     void wheelEvent(QWheelEvent *event);

signals:
     void mouseMoved(QVector2D);

     void mousePressed(QVector2D);

     void mouseReleased(QVector2D);

     void mouseWheelMoved(QWheelEvent *event);

};

#endif // MOUSENAVIGATOR_H
