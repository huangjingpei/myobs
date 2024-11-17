#ifndef GBSADDBROKER_H
#define GBSADDBROKER_H
#include <QMouseEvent>
#include <QDialog>
#include <QPoint>

namespace Ui {
class GBSAddBroker;
}

class GBSAddBroker : public QDialog
{
    Q_OBJECT

public:
    explicit GBSAddBroker(QWidget *parent = nullptr);
    ~GBSAddBroker();

private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Ui::GBSAddBroker *ui;
	QPoint dragPosition;
};

#endif // GBSADDBROKER_H
