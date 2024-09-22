#include <QWidget>
#include <QPoint>
#include <QSize>
#include <QRect>
#include <QFrame>
#include <QGridLayout>

#include "../resizableframe.h"

struct{
    bool leftResize = false;
    bool topResize = false;
    bool rightResize = false;
    bool bottomResize = false;
} resizeMethod;

struct pointPairXY {
    QPoint xPosPair;
    QPoint yPosPair;
};

class GridItemMover
{
private:
    QFrame *frame;
    QWidget *dragWidget;
    QSize dragWidgetSize;
    QPoint dragStartPos;
    QPoint dragWidgetPos;
    QPoint dragWidgetStartPositionLocalPosTopLeft;
    QPoint dragWidgetStartPositionLocalPosBottomRight;
    const int WIDGETMARGIN = 20;

public:
    GridItemMover();
    void handleMousePressEvent(QMouseEvent *event, QFrame *frame);
    void setWidgetProperties(const QGridLayout &gridLayout, QWidget *widget);
    void handleMouseMoveEvent(QMouseEvent *event);
    void handleMouseReleaseEvent(QMouseEvent *event);

private:
    void checkClickedOnWhichSideOfTheWidget(const QPoint &localPos, const QRect &widgetRect);
    void handleWidgetResizeOrMove(QMouseEvent *event, QWidget *child);
    void resizeWidgetBasedOnMouseMovement(QMouseEvent *event);
    void moveWidget(QMouseEvent *event);
    void resetGridLayout(QGridLayout *gridLayout);
    void adjustWidgetsRowColumn(const QGridLayout &gridLayout, QList<QWidget*> widgets);
    void changeRow(QList<QWidget*> widgets, const int &row);
    void changeColumn(QList<QWidget*> widgets, const int &column);
    void removeWidgetsFromGrid(QGridLayout *gridLayout, QList<QWidget*> widgets);
    void setWidgetsOnGrid(QGridLayout *gridLayout, QList<QWidget*> widgets);
    bool checkIfObjectIsInRowCol(const QGridLayout &gridLayout, const QPoint &movedDragWidgetLocalPosTopLeft, const QPoint &movedDragWidgetLocalPosBottomRight);
    bool checkIfIntervall(const QPoint& xPosPair1, const QPoint& xPosPair2, const QPoint& yPosPair1, const QPoint& yPosPair2);
    bool isItemInColumn(const QGridLayout &gridLayout, const int &index);
    bool isItemInRow(const QGridLayout &gridLayout, const int &index);
    int getGridCellWidth(const QGridLayout &gridLayout);
    int calculateGridItemDistance(const QGridLayout &gridLayout, const int &startPos, const int &endPos);
    int getGridCellHeight(const QGridLayout &gridLayout);
    QList<QWidget*> saveGridWidgetsToList(const QGridLayout &gridLayout);
};