#include "../include/mainwindow.h"
#include "../include/ui/ui_prototype_home_assistant_hub.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , gridItemMover(new GridItemMover)
{
    ui->setupUi(this);

    QGridLayout *gridLayout = qobject_cast<QGridLayout *>(ui->frame_main->layout());
    if (gridLayout) {
        ResizableFrame *frame1 = new ResizableFrame();
        frame1->farbe = "blue";
        frame1->setStyleSheet("background: blue");
        gridLayout->addWidget(frame1, 0, 0);

        ResizableFrame *frame2 = new ResizableFrame();
        frame2->farbe = "yellow";
        frame2->setStyleSheet("background: yellow");
        gridLayout->addWidget(frame2, 0, 1);

        ResizableFrame *frame3 = new ResizableFrame();
        frame3->farbe = "red";
        frame3->setStyleSheet("background: red");
        gridLayout->addWidget(frame3, 1, 1);

        ResizableFrame *frame4 = new ResizableFrame();
        frame4->farbe = "black";
        frame4->setStyleSheet("background: black");
        gridLayout->addWidget(frame4, 2, 2);

        ResizableFrame *frame5 = new ResizableFrame();
        frame5->farbe = "purple";
        frame5->setStyleSheet("background: purple");
        gridLayout->addWidget(frame5, 1, 3);

        gridItemMover->setWidgetProperties(*gridLayout, frame1);
        gridItemMover->setWidgetProperties(*gridLayout, frame2);
        gridItemMover->setWidgetProperties(*gridLayout, frame3);
        gridItemMover->setWidgetProperties(*gridLayout, frame4);
        gridItemMover->setWidgetProperties(*gridLayout, frame5);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    gridItemMover->handleMousePressEvent(event, ui->frame_main);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    gridItemMover->handleMouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    gridItemMover->handleMouseReleaseEvent(event);
    gridItemMover = new GridItemMover();
}





