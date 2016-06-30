        #ifndef QLABELMOUSEEVENTS_H
    #define QLABELMOUSEEVENTS_H

    #include <QObject>
    #include <QWidget>
    #include <QLabel>
    #include <QMouseEvent>
    #include <QEvent>

    class QLabelMouseEvents : public QLabel
    {
        Q_OBJECT
    public:
        explicit QLabelMouseEvents(QWidget *parent = 0 );

        void mouseMoveEvent(QMouseEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);


    signals:
        void mouseMoved(QVector2D);
        void mousePressed(QVector2D);
        void mouseReleased(QVector2D);


    };

    #endif // MOUSEEVENTS_H
