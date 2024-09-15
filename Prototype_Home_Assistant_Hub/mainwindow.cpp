#include "mainwindow.h"
#include "./ui_prototype_home_assistant_hub.h"

QSize drag;
QPoint dragPos;
bool leftResize = false;
bool topResize = false;
bool rightResize = false;
bool bottomResize = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , dragWidget(nullptr)
{
    ui->setupUi(this);

    QGridLayout *gridLayout = qobject_cast<QGridLayout *>(ui->frame_main->layout());
    if (gridLayout) {
        ResizableFrame *frame1 = new ResizableFrame();
        frame1->setStyleSheet("background: blue");
        gridLayout->addWidget(frame1, 0, 0);

        ResizableFrame *frame2 = new ResizableFrame();
        frame2->setStyleSheet("background: yellow");
        gridLayout->addWidget(frame2, 0, 1);

        ResizableFrame *frame3 = new ResizableFrame();
        frame3->setStyleSheet("background: red");
        gridLayout->addWidget(frame3, 1, 1);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QWidget *child = ui->frame_main->childAt(event->pos() - ui->frame_main->pos());
        if (child && dynamic_cast<ResizableFrame*>(child)) {
            const int margin = 20; // Abstand von den Rändern in Pixeln

            QRect widgetRect = child->rect();
            QPoint localPos = event->pos() - child->pos();

            // Bestimme, ob du an der linken oder oberen Seite klickst
            leftResize = localPos.x() <= margin;
            topResize = localPos.y() <= margin;
            rightResize = localPos.x() >= widgetRect.width() - margin;
            bottomResize = localPos.y() >= widgetRect.height() - margin;

            // Überprüfe, ob der Klick an einer Ecke oder Kante ist, um Resize zu aktivieren
            if (leftResize || rightResize || topResize || bottomResize) 
            {
                resizeMode = true;
                dragWidget = child;
                drag = dragWidget->size();
                dragPos = dragWidget->pos();
                dragStartPos = event->pos();
            }
            else {
                dragWidget = child;
                dragStartPos = event->pos();
                resizeMode = false;
            }
            dragWidget->raise();

            QGridLayout *gridLayout = qobject_cast<QGridLayout*>(ui->frame_main->layout());
            if (gridLayout) {
                int row, column, rowSpan, columnSpan;
                gridLayout->getItemPosition(gridLayout->indexOf(dragWidget), &row, &column, &rowSpan, &columnSpan);
                dragWidget->setProperty("gridRow", row);
                dragWidget->setProperty("gridColumn", column);
                dragWidget->setProperty("gridRowSpan", rowSpan);
                dragWidget->setProperty("gridColumnSpan", columnSpan);
            }
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (dragWidget && (event->buttons() & Qt::LeftButton)) {
        if(resizeMode)
        {
            QPoint diff = event->pos() - dragStartPos;
            QSize newSize = drag + QSize(diff.x(), diff.y());

            if (leftResize) {
                // Wenn wir von der linken Seite ziehen, ändere die Position und Größe
                int newX = dragPos.x() + diff.x();
                int newWidth = drag.width() - diff.x();
                dragWidget->setGeometry(newX, dragPos.y(), newWidth, drag.height());
            }
            else if (rightResize) {
                // Wenn wir von der rechten Seite ziehen, ändere die Breite
                int newWidth = drag.width() + diff.x();
                dragWidget->resize(newWidth, drag.height());
            }
            else if (topResize) {
                // Wenn wir von der oberen Seite ziehen, ändere die Position und Größe
                int newY = dragPos.y() + diff.y();
                int newHeight = drag.height() - diff.y();
                dragWidget->setGeometry(dragPos.x(), newY, drag.width(), newHeight);
            }
            else if (bottomResize) {
                // Wenn wir von der unteren Seite ziehen, ändere die Höhe
                int newHeight = drag.height() + diff.y();
                dragWidget->resize(drag.width(), newHeight);
            }
        }
        else {
            // Verschieben des Widgets
            QPoint diff = event->pos() - dragStartPos;
            QPoint newPos = dragWidget->pos() + diff;
            dragWidget->move(newPos);
            dragStartPos = event->pos();
        }
    }
}

int MainWindow::calculateGridItemDistance(const QPoint &pointA, const QPoint &pointB, const QGridLayout &gridLayout, const std::string &resizeMethod)
{
    int cellWidth = ui->frame_main->width() / gridLayout.columnCount();
    int cellHeight = ui->frame_main->height() / gridLayout.rowCount();

    int distance = 1;
    if(resizeMethod == "LEFT" || resizeMethod == "RIGHT"){
        int columnA = pointA.x() / cellWidth;
        int columnB = pointB.x() / cellWidth;
        distance = std::abs(columnA - columnB);
    }
    else if(resizeMethod == "TOP" || resizeMethod == "BOTTOM"){
        int rowB = pointB.y() / cellHeight;
        int rowA = pointA.y() / cellHeight;
        distance = std::abs(rowA - rowB);
    }

    if(distance == 0){
        distance = 1;
    }

    return distance;
}

void MainWindow::resetGridLayout(QGridLayout *gridLayout){
    QList<ResizableFrame*> widgets;
    for (int i = 0; i < gridLayout->rowCount(); ++i) {
        for (int j = 0; j < gridLayout->columnCount(); ++j) {
            QLayoutItem *item = gridLayout->itemAtPosition(i, j);
            if (item) {
                QWidget *widget = item->widget();
                if (ResizableFrame *resizableFrame = qobject_cast<ResizableFrame*>(widget)) {
                    int row, column, rowSpan, columnSpan;
                    gridLayout->getItemPosition(gridLayout->indexOf(resizableFrame), &row, &column, &rowSpan, &columnSpan);
                    resizableFrame->setProperty("gridRow", row);
                    resizableFrame->setProperty("gridColumn", column);
                    resizableFrame->setProperty("gridRowSpan", rowSpan);
                    resizableFrame->setProperty("gridColumnSpan", columnSpan);

                    gridLayout->removeWidget(resizableFrame);
                    resizableFrame->setParent(nullptr);
                    widgets.append(resizableFrame); 
                }
            }
        }
    }

    delete gridLayout;
    gridLayout = new QGridLayout();
    ui->frame_main->setLayout(gridLayout);
    
    for (int i = 0; i < widgets.size(); ++i) {
        gridLayout->addWidget(widgets[i],
                              widgets[i]->property("gridRow").toInt(),
                              widgets[i]->property("gridColumn").toInt(),
                              widgets[i]->property("gridRowSpan").toInt(),
                              widgets[i]->property("gridColumnSpan").toInt());
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    QGridLayout *gridLayout = qobject_cast<QGridLayout*>(ui->frame_main->layout());
    resetGridLayout(gridLayout);
    qDebug() << "---------------------------------------------------------";
    if (event->button() == Qt::LeftButton) {
        if (dragWidget) {
            QGridLayout *gridLayout = qobject_cast<QGridLayout*>(ui->frame_main->layout());
            qDebug() << "GRIDLAYOUT ROWS: " << gridLayout->rowCount() << " COLS: " << gridLayout->columnCount();
            if (gridLayout) {
                QPoint globalPos, layoutPos;
                if(rightResize || bottomResize){
                    globalPos = dragWidget->mapToGlobal(dragWidget->rect().bottomRight());
                    layoutPos = ui->frame_main->mapFromGlobal(globalPos);
                }
                else{
                    globalPos = dragWidget->mapToGlobal(dragWidget->rect().topLeft());
                    layoutPos = ui->frame_main->mapFromGlobal(globalPos); 
                }
                qDebug() << "Layoutpos: " << layoutPos;
                qDebug() << "Globalpos: " << globalPos;
                
                int column, row, columnSpan = 1, rowSpan = 1;
                int cellWidth = ui->frame_main->width() / gridLayout->columnCount();
                int cellHeight = ui->frame_main->height() / gridLayout->rowCount();

                column = layoutPos.x() / cellWidth;
                row = layoutPos.y() / cellHeight;

                columnSpan = dragWidget->property("gridColumnSpan").toInt();
                rowSpan = dragWidget->property("gridRowSpan").toInt();

                if(gridLayout->itemAtPosition(row, column)){
                    QLayoutItem *item = gridLayout->itemAtPosition(row, column);
                    //Wenn columnspan oder rowspan, prüfe, ob in jeder Kachel daneben kein Item vorhanden ist!
                    //Momentan prüft es nur da wo ich mit der Maus hinziehe und wenn diese eine leere ist, dann schiebt es sich darein, auch, wenn
                    //die Kachel daneben drunter drüber ein Item enthält 
                    if (item) {
                        QWidget *widget = item->widget();
                        qDebug() << "ITEMATPOSITION: ITEM: " << widget <<  " ROW: " << row << " COL: " << column;
                        if (ResizableFrame *resizableFrame = qobject_cast<ResizableFrame*>(widget)) {
                            if(!(resizableFrame == dragWidget)){
                                qDebug() << "Das Widget ist ein ResizableFrame!";
                                column = dragWidget->property("gridColumn").toInt();
                                row = dragWidget->property("gridRow").toInt();
                            }
                            if(rightResize){
                                qDebug() << "RIGHTRESIZE";
                                columnSpan = calculateGridItemDistance(layoutPos, dragPos, *gridLayout, "RIGHT");
                                column = dragWidget->property("gridColumn").toInt();
                                row = dragWidget->property("gridRow").toInt();
                            }
                            else if(leftResize){
                                qDebug() << "LEFTRESIZE";
                                columnSpan = calculateGridItemDistance(layoutPos, dragPos, *gridLayout, "LEFT");
                            }
                            else if(bottomResize){
                                qDebug() << "BOTTOMRESIZE";
                                rowSpan = calculateGridItemDistance(layoutPos, dragPos, *gridLayout, "BOTTOM");
                                column = dragWidget->property("gridColumn").toInt();
                                row = dragWidget->property("gridRow").toInt();
                            }
                            else if(topResize){
                                qDebug() << "TOPRESIZE";
                                rowSpan = calculateGridItemDistance(layoutPos, dragPos, *gridLayout, "TOP");
                            }
                        } else {
                            qDebug() << "Das Widget ist kein ResizableFrame.";
                        }
                    }
                }
                else{
                   if(rightResize){
                        qDebug() << "RIGHTRESIZE";
                        columnSpan = columnSpan + calculateGridItemDistance(layoutPos, dragPos, *gridLayout, "RIGHT");
                        column = dragWidget->property("gridColumn").toInt();
                        row = dragWidget->property("gridRow").toInt();
                    }
                    else if(leftResize){
                        qDebug() << "LEFTRESIZE";
                        columnSpan = columnSpan + calculateGridItemDistance(layoutPos, dragPos, *gridLayout, "LEFT");
                    }
                    else if(bottomResize){
                        qDebug() << "BOTTOMRESIZE";
                        rowSpan = rowSpan + calculateGridItemDistance(layoutPos, dragPos, *gridLayout, "BOTTOM");
                        column = dragWidget->property("gridColumn").toInt();
                        row = dragWidget->property("gridRow").toInt();
                    }
                    else if(topResize){
                        qDebug() << "TOPRESIZE";
                        rowSpan = rowSpan + calculateGridItemDistance(layoutPos, dragPos, *gridLayout, "TOP");
                    }
                }

                qDebug() << "ROW: " << row << " COL: " << column << " ROWSPAN: " << rowSpan << " COLSPAN: "<< columnSpan;
                gridLayout->removeWidget(dragWidget);
                gridLayout->addWidget(dragWidget, row, column, rowSpan, columnSpan);
                dragWidget->move(0, 0);
            }
        }
        resizeMode = false;
        leftResize = false;
        topResize = false;
        rightResize = false;
        bottomResize = false;
        dragWidget = nullptr;
    }
}