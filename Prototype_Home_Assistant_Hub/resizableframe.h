#ifndef RESIZABLEFRAME_H
#define RESIZABLEFRAME_H

#include <QFrame>
#include <QMouseEvent>
#include <QCursor>
#include <QDebug>

class ResizableFrame : public QFrame
{
    Q_OBJECT

public:
    ResizableFrame(QWidget *parent = nullptr);

};

#endif // RESIZABLEFRAME_H
