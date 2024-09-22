#include "../../include/gridItemMover/gridItemMover.h"

GridItemMover::GridItemMover() 
        : dragWidget(nullptr)
{

}

void GridItemMover::handleMousePressEvent(QMouseEvent *event, QFrame *frame){
    this->frame = frame;
    if (event->button() == Qt::LeftButton) {
        QWidget *child = this->frame->childAt(event->pos() - this->frame->pos());
        if (child && dynamic_cast<ResizableFrame*>(child)) {

            QRect widgetRect = child->rect();
            QPoint localPos = event->pos() - child->pos(); 
            checkClickedOnWhichSideOfTheWidget(localPos, widgetRect);

            handleWidgetResizeOrMove(event, child);

            QGridLayout *gridLayout = qobject_cast<QGridLayout*>(this->frame->layout());
            setWidgetProperties(*gridLayout, dragWidget);

            dragWidget->raise();
        }
    }
}

void GridItemMover::handleMouseMoveEvent(QMouseEvent *event)
{
    if (dragWidget && (event->buttons() & Qt::LeftButton)) {
        // Resize or move Widget
        if(resizeMethod.leftResize || resizeMethod.rightResize || resizeMethod.topResize || resizeMethod.bottomResize){
            resizeWidgetBasedOnMouseMovement(event);
        }
        else {
            moveWidget(event);
        }
    }
}

void GridItemMover::handleMouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (dragWidget) {
            QGridLayout *gridLayout = qobject_cast<QGridLayout*>(this->frame->layout());
            if (gridLayout) {
                QPoint movedDragWidgetLocalPosTopLeft = dragWidget->mapTo(this->frame, QPoint(0, 0));
                QPoint movedDragWidgetLocalPosBottomRight = dragWidget->mapTo(this->frame, QPoint(dragWidget->width(), dragWidget->height()));

                rowDragWidgetStartPosition = dragWidget->property("gridRow").toInt();
                columnDragWidgetStartPosition = dragWidget->property("gridColumn").toInt();
                rowSpanDragWidgetStartPosition = dragWidget->property("gridRowSpan").toInt();
                columnSpanDragWidgetStartPosition = dragWidget->property("gridColumnSpan").toInt();
                
                QPair<int, int> pair = calculateRowColumnPosition(*gridLayout, movedDragWidgetLocalPosTopLeft, movedDragWidgetLocalPosBottomRight);
                int rowMovedDragWidgetStartPosition = pair.first;
                int columnMovedDragWidgetStartPosition = pair.second;

                if(!checkIfObjectIsInRowCol(gridLayout, movedDragWidgetLocalPosTopLeft, movedDragWidgetLocalPosBottomRight)){
                    if(!resizeMethod.leftResize && !resizeMethod.rightResize && !resizeMethod.topResize && !resizeMethod.bottomResize){
                        // Draging without resizing!
                        gridLayout->removeWidget(dragWidget);
                        gridLayout->addWidget(dragWidget, rowMovedDragWidgetStartPosition, columnMovedDragWidgetStartPosition, rowSpanDragWidgetStartPosition, columnSpanDragWidgetStartPosition);
                        dragWidget->move(0, 0);
                    }
                    else{
                        // Resizing!
                        resizeAndPositionWidget(gridLayout, rowMovedDragWidgetStartPosition, columnMovedDragWidgetStartPosition);
                    }
                }
                else{
                    gridLayout->removeWidget(dragWidget);
                    gridLayout->addWidget(dragWidget, rowDragWidgetStartPosition, columnDragWidgetStartPosition, rowSpanDragWidgetStartPosition, columnSpanDragWidgetStartPosition);
                }
            }
            setWidgetProperties(*gridLayout, dragWidget);
            resetGridLayout(gridLayout);
        }
        resizeMethod.leftResize = false;
        resizeMethod.topResize = false;
        resizeMethod.rightResize = false;
        resizeMethod.bottomResize = false;
        dragWidget = nullptr;
    }
}

QPair<int, int> GridItemMover::calculateRowColumnPosition(const QGridLayout &gridLayout, QPoint movedDragWidgetLocalPosTopLeft, QPoint movedDragWidgetLocalPosBottomRight)
{
    int cellWidth = getGridCellWidth(gridLayout);
    int cellHeight = getGridCellHeight(gridLayout);

    int rowMovedDragWidgetStartPosition;
    int columnMovedDragWidgetStartPosition;

    if(resizeMethod.rightResize){
        rowMovedDragWidgetStartPosition = rowDragWidgetStartPosition;
        columnMovedDragWidgetStartPosition = movedDragWidgetLocalPosBottomRight.x() / (cellWidth + gridLayout.horizontalSpacing());
    }
    else if(resizeMethod.leftResize){
        rowMovedDragWidgetStartPosition = rowDragWidgetStartPosition;
        columnMovedDragWidgetStartPosition = movedDragWidgetLocalPosTopLeft.x() / (cellWidth + gridLayout.horizontalSpacing());
    }
    else if(resizeMethod.topResize){
        rowMovedDragWidgetStartPosition = movedDragWidgetLocalPosTopLeft.y() / (cellHeight + gridLayout.verticalSpacing());
        columnMovedDragWidgetStartPosition = columnDragWidgetStartPosition;
    }
    else if(resizeMethod.bottomResize){
        rowMovedDragWidgetStartPosition = movedDragWidgetLocalPosBottomRight.y() / (cellHeight + gridLayout.verticalSpacing());
        columnMovedDragWidgetStartPosition = columnDragWidgetStartPosition;
    }
    else{
        //rowMovedDragWidgetStartPosition = movedDragWidgetLocalPosTopLeft.y() / (cellHeight + gridLayout.verticalSpacing());
        //columnMovedDragWidgetStartPosition = movedDragWidgetLocalPosTopLeft.x() / (cellWidth + gridLayout.horizontalSpacing());
        if(movedDragWidgetLocalPosTopLeft == dragWidgetStartPositionLocalPosTopLeft && movedDragWidgetLocalPosBottomRight == dragWidgetStartPositionLocalPosBottomRight){
            rowMovedDragWidgetStartPosition = rowDragWidgetStartPosition;
            columnMovedDragWidgetStartPosition = columnDragWidgetStartPosition;
        }
        else{
            rowMovedDragWidgetStartPosition = ((movedDragWidgetLocalPosTopLeft.y() + movedDragWidgetLocalPosBottomRight.y()) / 2) / (cellHeight + gridLayout.verticalSpacing());
            columnMovedDragWidgetStartPosition = ((movedDragWidgetLocalPosTopLeft.x() + movedDragWidgetLocalPosBottomRight.x()) / 2) / (cellWidth + gridLayout.horizontalSpacing());
        }
    }

    if(rowMovedDragWidgetStartPosition < 0){
        rowMovedDragWidgetStartPosition = dragWidget->property("gridRow").toInt();
    }
    if(columnMovedDragWidgetStartPosition < 0){
        columnMovedDragWidgetStartPosition = dragWidget->property("gridColumn").toInt();
    }


    return QPair<int, int>(rowMovedDragWidgetStartPosition, columnMovedDragWidgetStartPosition);
}

void GridItemMover::checkClickedOnWhichSideOfTheWidget(const QPoint &localPos, const QRect &widgetRect)
{
    resizeMethod.leftResize = localPos.x() <= WIDGETMARGIN;
    if(resizeMethod.leftResize)
        return;

    resizeMethod.topResize = localPos.y() <= WIDGETMARGIN;
    if(resizeMethod.topResize)
        return;

    resizeMethod.rightResize = localPos.x() >= widgetRect.width() - WIDGETMARGIN;
    if(resizeMethod.rightResize)
        return;

    resizeMethod.bottomResize = localPos.y() >= widgetRect.height() - WIDGETMARGIN;
    if(resizeMethod.bottomResize)
        return;
}

void GridItemMover::handleWidgetResizeOrMove(QMouseEvent *event, QWidget *child)
{
    if (resizeMethod.leftResize || resizeMethod.rightResize || resizeMethod.topResize || resizeMethod.bottomResize) {
        dragWidget = child;
        dragWidgetSize = dragWidget->size();
        dragWidgetPos = dragWidget->pos();
        dragStartPos = event->pos();
    }
    else {
        dragWidget = child;
        dragStartPos = event->pos();
    }
    dragWidgetStartPositionLocalPosTopLeft = dragWidget->mapTo(this->frame, QPoint(0, 0));
    dragWidgetStartPositionLocalPosBottomRight = dragWidget->mapTo(this->frame, QPoint(dragWidget->width(), dragWidget->height()));
}

void GridItemMover::setWidgetProperties(const QGridLayout &gridLayout, QWidget *widget)
{
    int row, column, rowSpan, columnSpan;
    gridLayout.getItemPosition(gridLayout.indexOf(widget), &row, &column, &rowSpan, &columnSpan);
    widget->setProperty("gridRow", row);
    widget->setProperty("gridColumn", column);
    widget->setProperty("gridRowSpan", rowSpan);
    widget->setProperty("gridColumnSpan", columnSpan);
    ResizableFrame *frame = qobject_cast<ResizableFrame*>(widget);
}

void GridItemMover::resizeWidgetBasedOnMouseMovement(QMouseEvent *event)
{
    QPoint diff = event->pos() - dragStartPos;
    QSize newSize = dragWidgetSize + QSize(diff.x(), diff.y());

    if (resizeMethod.leftResize) {
        int newX = dragWidgetPos.x() + diff.x();
        int newWidth = dragWidgetSize.width() - diff.x();
        dragWidget->setGeometry(newX, dragWidgetPos.y(), newWidth, dragWidgetSize.height());
    }
    else if (resizeMethod.rightResize) {
        int newWidth = dragWidgetSize.width() + diff.x();
        dragWidget->resize(newWidth, dragWidgetSize.height());
    }
    else if (resizeMethod.topResize) {
        int newY = dragWidgetPos.y() + diff.y();
        int newHeight = dragWidgetSize.height() - diff.y();
        dragWidget->setGeometry(dragWidgetPos.x(), newY, dragWidgetSize.width(), newHeight);
    }
    else if (resizeMethod.bottomResize) {
        int newHeight = dragWidgetSize.height() + diff.y();
        dragWidget->resize(dragWidgetSize.width(), newHeight);
    }
}

void GridItemMover::moveWidget(QMouseEvent *event)
{
    QPoint diff = event->pos() - dragStartPos;
    QPoint newPos = dragWidget->pos() + diff;
    dragWidget->move(newPos);
    dragStartPos = event->pos();
}

int GridItemMover::getGridCellWidth(const QGridLayout &gridLayout)
{
    return (this->frame->width() - ((gridLayout.columnCount() - 1) * gridLayout.horizontalSpacing())) / gridLayout.columnCount();
}

int GridItemMover::getGridCellHeight(const QGridLayout &gridLayout)
{
    return (this->frame->height() - ((gridLayout.rowCount() - 1) * gridLayout.verticalSpacing())) / gridLayout.rowCount();
}

void GridItemMover::resizeAndPositionWidget(QGridLayout *gridLayout, int rowMovedDragWidgetStartPosition, int columnMovedDragWidgetStartPosition)
{
    int cellWidth = getGridCellWidth(*gridLayout);
    int cellHeight = getGridCellHeight(*gridLayout);

    if(resizeMethod.leftResize){
        int column = (dragWidgetStartPositionLocalPosBottomRight.x() - (gridLayout->columnCount() * gridLayout->horizontalSpacing())) / cellWidth;
        int columnSpan = calculateGridItemDistance(*gridLayout, columnMovedDragWidgetStartPosition, column);
        gridLayout->addWidget(dragWidget, rowMovedDragWidgetStartPosition, columnMovedDragWidgetStartPosition, rowSpanDragWidgetStartPosition, columnSpan);
    }
    else if(resizeMethod.rightResize){
        int columnSpan = calculateGridItemDistance(*gridLayout, columnDragWidgetStartPosition, columnMovedDragWidgetStartPosition);
        gridLayout->addWidget(dragWidget, rowDragWidgetStartPosition, columnDragWidgetStartPosition, rowSpanDragWidgetStartPosition, columnSpan);
    }
    else if(resizeMethod.bottomResize){
        int rowSpan = calculateGridItemDistance(*gridLayout, rowDragWidgetStartPosition, rowMovedDragWidgetStartPosition);
        gridLayout->addWidget(dragWidget, rowDragWidgetStartPosition, columnDragWidgetStartPosition, rowSpan, columnSpanDragWidgetStartPosition);
    }
    else if(resizeMethod.topResize){
        int row = (dragWidgetStartPositionLocalPosBottomRight.y() - (gridLayout->rowCount() * gridLayout->verticalSpacing())) / cellHeight;
        int rowSpan = calculateGridItemDistance(*gridLayout, rowMovedDragWidgetStartPosition, row);
        gridLayout->addWidget(dragWidget, rowMovedDragWidgetStartPosition, columnMovedDragWidgetStartPosition, rowSpan, columnSpanDragWidgetStartPosition);
    }
}

QList<QPair<int, int>> GridItemMover::calculateOccupiedCellsFromFrames(const QGridLayout &gridLayout, bool getDragWidgetCells)
{
    QList<QPair<int, int>> occupiedCells;

    for (int i = 0; i < gridLayout.count(); ++i) {
        QLayoutItem *item = gridLayout.itemAt(i);
        if (item) {
            QWidget *widget = item->widget();
            if(ResizableFrame *resizableFrameWidget = qobject_cast<ResizableFrame*>(widget)){
                int row, column, rowSpan, columnSpan;
                gridLayout.getItemPosition(i, &row, &column, &rowSpan, &columnSpan);

                for (int r = row; r < row + rowSpan; ++r) {
                    for (int c = column; c < column + columnSpan; ++c) {
                        if(resizableFrameWidget != dragWidget && !getDragWidgetCells){
                            occupiedCells.append(QPair<int, int>(r, c));
                        }
                        else if(resizableFrameWidget == dragWidget && getDragWidgetCells){
                            occupiedCells.append(QPair<int, int>(r, c));
                        }
                    }
                }
            }
        }
    }
    return occupiedCells;
}

bool GridItemMover::checkIfObjectIsInRowCol(QGridLayout *gridLayout, const QPoint &movedDragWidgetLocalPosTopLeft, const QPoint &movedDragWidgetLocalPosBottomRight)
{
    QPoint x = QPoint(movedDragWidgetLocalPosTopLeft.x(), movedDragWidgetLocalPosBottomRight.x());
    QPoint y = QPoint(movedDragWidgetLocalPosTopLeft.y(), movedDragWidgetLocalPosBottomRight.y());
    pointPairXY movedWidgetPair = {x,y};

    if(movedWidgetPair.xPosPair.x() == movedWidgetPair.xPosPair.y() || movedWidgetPair.yPosPair.x() == movedWidgetPair.yPosPair.y()){
        return true;
    }

    QPair<int, int> pair = calculateRowColumnPosition(*gridLayout, movedDragWidgetLocalPosTopLeft, movedDragWidgetLocalPosBottomRight);
    int rowMovedDragWidgetStartPosition = pair.first;
    int columnMovedDragWidgetStartPosition = pair.second;

    gridLayout->removeWidget(dragWidget);
    if(resizeMethod.leftResize || resizeMethod.rightResize || resizeMethod.topResize || resizeMethod.bottomResize){
        resizeAndPositionWidget(gridLayout, rowMovedDragWidgetStartPosition, columnMovedDragWidgetStartPosition);
    }
    else{
        gridLayout->addWidget(dragWidget, rowMovedDragWidgetStartPosition, columnMovedDragWidgetStartPosition, rowSpanDragWidgetStartPosition, columnSpanDragWidgetStartPosition);
    }
    
    QList<QPair<int, int>> occupiedCells = calculateOccupiedCellsFromFrames(*gridLayout, false);
    QList<QPair<int, int>> occupiedCellsDragWidget = calculateOccupiedCellsFromFrames(*gridLayout, true);

    for (int i = 0; i < occupiedCells.size(); i++) {
        for(int j = 0; j < occupiedCellsDragWidget.size(); j++){
            if (occupiedCellsDragWidget[j].first == occupiedCells[i].first && occupiedCellsDragWidget[j].second == occupiedCells[i].second){
                return true;
            }
        }
    }

    return false;
}

bool GridItemMover::checkIfIntervall(const QPoint& xPosPair1, const QPoint& xPosPair2, const QPoint& yPosPair1, const QPoint& yPosPair2)
{
    bool inIntervallX = (xPosPair1.x() <= xPosPair2.y() && xPosPair1.y() >= xPosPair2.x());
    bool inIntervallY = (yPosPair1.x() <= yPosPair2.y() && yPosPair1.y() >= yPosPair2.x());
    return inIntervallX && inIntervallY;
}

int GridItemMover::calculateGridItemDistance(const QGridLayout &gridLayout, const int &startPos, const int &endPos)
{
    int cellWidth = getGridCellWidth(gridLayout);
    int cellHeight = getGridCellHeight(gridLayout);

    int distance = 1;
    distance = std::abs(startPos - endPos) + 1;

    return distance;
}

/*
 *Bug with the purple widget if you drag the black one and then place it over it.
 *The purple widget changes its position although this should not be the case
*/
void GridItemMover::resetGridLayout(QGridLayout *gridLayout)
{
    QList<QWidget*> widgets = saveGridWidgetsToList(*gridLayout);
    adjustWidgetsRowColumn(*gridLayout, widgets);
    removeWidgetsFromGrid(gridLayout, widgets);

    delete gridLayout;
    gridLayout = new QGridLayout();
    gridLayout->setContentsMargins(0, 0, 0, 0);
    this->frame->setLayout(gridLayout);

    setWidgetsOnGrid(gridLayout, widgets);

    qDebug() << "-------1------";
    for (int i = 0; i < gridLayout->rowCount(); ++i) {
        for (int j = 0; j < gridLayout->columnCount(); ++j) {
            QLayoutItem *item = gridLayout->itemAtPosition(i, j);
            if (item) {
                QWidget *widget = item->widget();
                if (ResizableFrame *resizableFrame = qobject_cast<ResizableFrame*>(widget)) {
                    qDebug() << "frame: " << resizableFrame->farbe << " row: " << resizableFrame->property("gridRow").toString() << " column: " << resizableFrame->property("gridColumn").toString() << " rowspan: " << resizableFrame->property("gridRowSpan").toString() << " columnspan: " << resizableFrame->property("gridColumnSpan").toString();
                }
            }
        }
    }
    qDebug() << "-------2------";  
}

QList<QWidget*> GridItemMover::saveGridWidgetsToList(const QGridLayout &gridLayout)
{
    QList<QWidget*> widgets;
    for (int i = 0; i < gridLayout.rowCount(); ++i) {
        for (int j = 0; j < gridLayout.columnCount(); ++j) {
            QLayoutItem *item = gridLayout.itemAtPosition(i, j);
            if (item) {
                QWidget *widget = item->widget();
                if (ResizableFrame *resizableFrame = qobject_cast<ResizableFrame*>(widget)) {
                    qDebug() << "frame: " << resizableFrame->farbe << " row: " << resizableFrame->property("gridRow").toString() << " column: " << resizableFrame->property("gridColumn").toString() << " rowspan: " << resizableFrame->property("gridRowSpan").toString() << " columnspan: " << resizableFrame->property("gridColumnSpan").toString();
                    setWidgetProperties(gridLayout, resizableFrame);
                    widgets.append(widget); 
                }
            }
        }
    }
    return widgets;
}

void GridItemMover::adjustWidgetsRowColumn(const QGridLayout &gridLayout, QList<QWidget*> widgets)
{
    for (int i = 0; i < gridLayout.rowCount(); i++){
        if(!isItemInColumn(gridLayout, i)){
            changeRow(widgets, i); 
        }
    }

    for (int i = 0; i < gridLayout.columnCount(); i++){
        if(!isItemInRow(gridLayout, i)){
            changeColumn(widgets, i); 
        }
    }
}

bool GridItemMover::isItemInColumn(const QGridLayout &gridLayout, const int &index)
{
    for (int j = 0; j < gridLayout.columnCount(); j++){
        if(gridLayout.itemAtPosition(index,j)){
            return true;
        }
    }
    return false;
}

void GridItemMover::changeRow(QList<QWidget*> widgets, const int &row)
{
    for (int i = 0; i < widgets.size(); i++)
    {
        if(widgets[i]->property("gridRow").toInt() >= row + 1){
            widgets[i]->setProperty("gridRow", widgets[i]->property("gridRow").toInt() - 1);
        }
    }
}

bool GridItemMover::isItemInRow(const QGridLayout &gridLayout, const int &index)
{
    for (int j = 0; j < gridLayout.rowCount(); j++){
        if(gridLayout.itemAtPosition(j, index)){
            return true;
        }
    }
    return false;
}

void GridItemMover::changeColumn(QList<QWidget*> widgets, const int &column)
{
    for (int i = 0; i < widgets.size(); i++)
    {
        if(widgets[i]->property("gridColumn").toInt() >= column + 1){
            widgets[i]->setProperty("gridColumn", widgets[i]->property("gridColumn").toInt() - 1);
        }
    }
}

void GridItemMover::removeWidgetsFromGrid(QGridLayout *gridLayout, QList<QWidget*> widgets)
{
    for (int i = 0; i < widgets.size(); i++)
    {
        gridLayout->removeWidget(widgets[i]);
        widgets[i]->setParent(nullptr);
    }
}

void GridItemMover::setWidgetsOnGrid(QGridLayout *gridLayout, QList<QWidget*> widgets)
{
    for (int i = 0; i < widgets.size(); ++i) {
        if(ResizableFrame *resizableFrame = qobject_cast<ResizableFrame*>(widgets[i])){
            gridLayout->addWidget(widgets[i],
                                  widgets[i]->property("gridRow").toInt(),
                                  widgets[i]->property("gridColumn").toInt(),
                                  widgets[i]->property("gridRowSpan").toInt(),
                                  widgets[i]->property("gridColumnSpan").toInt());
        }
    }
}