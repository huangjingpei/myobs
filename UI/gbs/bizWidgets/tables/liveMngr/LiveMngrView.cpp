#include "LiveMngrView.h"

LiveMngrView::LiveMngrView(QWidget *parent)
    : QTableView(parent)
{
}

void LiveMngrView::mousePressEvent(QMouseEvent *event)
{
    emit mousePressEventOccurred(event);
    QTableView::mousePressEvent(event);
}
