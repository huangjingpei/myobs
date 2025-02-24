#ifndef LiveMngrView_H
#define LiveMngrView_H

#include <QTableView>
#include <QMouseEvent>
#include <QModelIndex>

class LiveMngrView : public QTableView
{
    Q_OBJECT

public:
    LiveMngrView(QWidget *parent = nullptr);

signals:
    void mousePressEventOccurred(QMouseEvent *event);

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // LiveMngrView_H
