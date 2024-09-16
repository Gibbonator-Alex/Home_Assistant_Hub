#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QGridLayout>

#include "resizableframe.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QWidget *dragWidget;
    QPoint dragStartPos;
    bool resizeMode;

    int calculateGridItemDistance(const QPoint &pointA, const QPoint &pointB, const QGridLayout &gridLayout, const std::string &resizeMethod);
    void resetGridLayout(QGridLayout *gridLayout);
    bool checkIfObjectIsInRowCol(const QGridLayout *gridLayout, const int &row, const int &column);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // MAINWINDOW_H
