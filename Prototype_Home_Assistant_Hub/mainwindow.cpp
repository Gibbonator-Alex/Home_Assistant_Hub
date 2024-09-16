#include "mainwindow.h"
#include "./ui_prototype_home_assistant_hub.h"

QSize dragWidgetSize;
QPoint dragWidgetPos;
int dragWidgetStartPosWidth;
int dragWidgetStartPosHeight;

struct{
    const std::string LEFTRESIZETEXT = "LEFTRESIZE";
    const std::string RIGHTRESIZETEXT = "RIGHTRESIZE";
    const std::string BOTTOMRESIZETEXT = "BOTTOMRESIZE";
    const std::string TOPRESIZETEXT = "TOPRESIZE";
    bool leftResize = false;
    bool topResize = false;
    bool rightResize = false;
    bool bottomResize = false;
} resizeMethod;

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
            resizeMethod.leftResize = localPos.x() <= margin;
            resizeMethod.topResize = localPos.y() <= margin;
            resizeMethod.rightResize = localPos.x() >= widgetRect.width() - margin;
            resizeMethod.bottomResize = localPos.y() >= widgetRect.height() - margin;

            // Überprüfe, ob der Klick an einer Ecke oder Kante ist, um Resize zu aktivieren
            if (resizeMethod.leftResize || resizeMethod.rightResize || resizeMethod.topResize || resizeMethod.bottomResize) 
            {
                resizeMode = true;
                dragWidget = child;
                dragWidgetSize = dragWidget->size();
                dragWidgetPos = dragWidget->pos();
                dragWidgetStartPosWidth = dragWidget->width();
                dragWidgetStartPosHeight= dragWidget->height();
                qDebug() << "DRAGWIDGET HEIGHT: " << dragWidget->height() << "--------------";
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
            QSize newSize = dragWidgetSize + QSize(diff.x(), diff.y());

            if (resizeMethod.leftResize) {
                // Wenn wir von der linken Seite ziehen, ändere die Position und Größe
                int newX = dragWidgetPos.x() + diff.x();
                int newWidth = dragWidgetSize.width() - diff.x();
                dragWidget->setGeometry(newX, dragWidgetPos.y(), newWidth, dragWidgetSize.height());
            }
            else if (resizeMethod.rightResize) {
                // Wenn wir von der rechten Seite ziehen, ändere die Breite
                int newWidth = dragWidgetSize.width() + diff.x();
                dragWidget->resize(newWidth, dragWidgetSize.height());
            }
            else if (resizeMethod.topResize) {
                // Wenn wir von der oberen Seite ziehen, ändere die Position und Größe
                int newY = dragWidgetPos.y() + diff.y();
                int newHeight = dragWidgetSize.height() - diff.y();
                dragWidget->setGeometry(dragWidgetPos.x(), newY, dragWidgetSize.width(), newHeight);
            }
            else if (resizeMethod.bottomResize) {
                // Wenn wir von der unteren Seite ziehen, ändere die Höhe
                int newHeight = dragWidgetSize.height() + diff.y();
                dragWidget->resize(dragWidgetSize.width(), newHeight);
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

int MainWindow::calculateGridItemDistance(const QPoint &pointA, const QPoint &pointB, const QGridLayout &gridLayout, const std::string &resizeString)
{
    qDebug() << "POINTA: " << pointA << " POINTB: " << pointB;
    //int cellWidth = ui->frame_main->width() / gridLayout.columnCount();
    //int cellHeight = ui->frame_main->height() / gridLayout.rowCount();
    int cellWidth = (ui->frame_main->width() - (gridLayout.columnCount() - 1) * gridLayout.horizontalSpacing()) / gridLayout.columnCount();
    int cellHeight = (ui->frame_main->height() - (gridLayout.rowCount() - 1) * gridLayout.verticalSpacing()) / gridLayout.rowCount();
    qDebug() << "cellheight: " << cellHeight << "cellwidth: " << cellWidth;

    int distance = 1;
    if(resizeString == resizeMethod.LEFTRESIZETEXT || resizeString == resizeMethod.RIGHTRESIZETEXT){
        int columnA = pointA.x() / cellWidth;
        int columnB = pointB.x() / cellWidth;
        qDebug() << "COLA: " << columnA << " COLB: " << columnB;
        distance = std::abs(columnA - columnB);
    }
    else if(resizeString == resizeMethod.TOPRESIZETEXT || resizeString == resizeMethod.BOTTOMRESIZETEXT){
        int rowA = pointA.y() / cellHeight;
        int rowB = pointB.y() / cellHeight;
        qDebug() << "ROWA: " << rowA << " ROWB: " << rowB;
        distance = std::abs(rowA - rowB);
        qDebug() << "rowA" << rowA << "rowB: " << rowB << "abs:" <<std::abs(rowA - rowB) << " without: " << rowA-rowB;
    }
    distance++;

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

bool MainWindow::checkIfObjectIsInRowCol(const QGridLayout *gridLayout, const int &row, const int &column)
{
    if(gridLayout->itemAtPosition(row, column)){
        QLayoutItem *item = gridLayout->itemAtPosition(row, column);
        if (item) {
            QWidget *widget = item->widget();
            if (ResizableFrame *resizableFrame = qobject_cast<ResizableFrame*>(widget)) {
                if(!(resizableFrame == dragWidget)){
                    qDebug() << "Das Widget ist nicht das selbe ResizableFrame!";
                    return false;
                }
            } else {
                return true;
            }
        }
    }
    return true;
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
                if(resizeMethod.rightResize || resizeMethod.bottomResize){
                    qDebug() << "rightbottom";
                    globalPos = dragWidget->mapToGlobal(dragWidget->rect().bottomRight());
                    //layoutPos = ui->frame_main->mapFromGlobal(globalPos);
                    layoutPos = dragWidget->mapTo(ui->frame_main, QPoint(dragWidget->width(), dragWidget->height()));
                }
                else{
                    qDebug() << "lefttop";
                    globalPos = dragWidget->mapToGlobal(dragWidget->rect().topLeft());
                    //layoutPos = ui->frame_main->mapFromGlobal(globalPos);
                    layoutPos = dragWidget->mapTo(ui->frame_main, QPoint(0, 0));
                }
                qDebug() << "Layoutpos: " << layoutPos;
                qDebug() << "Globalpos: " << globalPos;
                
                int column, row, columnSpan = 1, rowSpan = 1;
                //int cellWidth = (ui->frame_main->width() / gridLayout->columnCount());
                //int cellHeight = (ui->frame_main->height() / gridLayout->rowCount());

                int cellWidth = (ui->frame_main->width() - (gridLayout->columnCount() - 1) * gridLayout->horizontalSpacing()) / gridLayout->columnCount();
                int cellHeight = (ui->frame_main->height() - (gridLayout->rowCount() - 1) * gridLayout->verticalSpacing()) / gridLayout->rowCount();

                qDebug() << "cellheight: " << cellHeight << "cellwidth: " << cellWidth << " frame height: " << ui->frame_main->height() << " rowcount: " << gridLayout->rowCount();

                column = layoutPos.x() / cellWidth;
                if(column < 0){
                    qDebug() << "incol";
                    column = dragWidget->property("gridColumn").toInt();
                }
                row = layoutPos.y() / cellHeight;
                if(row < 0){
                    qDebug() << "inrow";
                    row = dragWidget->property("gridRow").toInt();
                }
                qDebug() << "ROW: " << row << " COL: " << column;

                columnSpan = dragWidget->property("gridColumnSpan").toInt();
                rowSpan = dragWidget->property("gridRowSpan").toInt();

                if(checkIfObjectIsInRowCol(gridLayout, row, column)){
                    if(resizeMethod.rightResize){
                        qDebug() << "RIGHTRESIZE";
                        columnSpan = calculateGridItemDistance(layoutPos, dragWidgetPos, *gridLayout, resizeMethod.RIGHTRESIZETEXT);
                        column = dragWidget->property("gridColumn").toInt();
                        row = dragWidget->property("gridRow").toInt();
                    }
                    else if(resizeMethod.leftResize){
                        qDebug() << "LEFTRESIZE";
                        QPoint positionA = ui->frame_main->mapToGlobal(event->pos());
                        QPoint positionB(dragWidgetPos.x() + dragWidgetStartPosWidth, dragWidgetPos.y());
                        columnSpan = calculateGridItemDistance(positionA, positionB, *gridLayout, resizeMethod.LEFTRESIZETEXT);
                    }
                    else if(resizeMethod.bottomResize){
                        qDebug() << "BOTTOMRESIZE";
                        rowSpan = calculateGridItemDistance(layoutPos, dragWidgetPos, *gridLayout, resizeMethod.BOTTOMRESIZETEXT);
                        column = dragWidget->property("gridColumn").toInt();
                        row = dragWidget->property("gridRow").toInt();
                    }
                    else if(resizeMethod.topResize){
                        qDebug() << "TOPRESIZE";
                        QPoint positionA = ui->frame_main->mapToGlobal(event->pos());
                        QPoint positionB(dragWidgetPos.x() + dragWidgetStartPosWidth, dragWidgetPos.y() + dragWidgetStartPosHeight);
                        qDebug() << "StartpositionHeight" << dragWidgetStartPosHeight;
                        rowSpan = calculateGridItemDistance(positionA, positionB, *gridLayout, resizeMethod.TOPRESIZETEXT);
                    }
                }
                else{
                    column = dragWidget->property("gridColumn").toInt();
                    row = dragWidget->property("gridRow").toInt();
                }

                qDebug() << "ROW: " << row << " COL: " << column << " ROWSPAN: " << rowSpan << " COLSPAN: "<< columnSpan;
                gridLayout->removeWidget(dragWidget);
                gridLayout->addWidget(dragWidget, row, column, rowSpan, columnSpan);
                dragWidget->move(0, 0);
            }
        }
        resizeMode = false;
        resizeMethod.leftResize = false;
        resizeMethod.topResize = false;
        resizeMethod.rightResize = false;
        resizeMethod.bottomResize = false;
        dragWidget = nullptr;
    }
}