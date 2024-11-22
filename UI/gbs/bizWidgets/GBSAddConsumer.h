#ifndef __GBS_ADD_CONSUMER_H__
#define __GBS_ADD_CONSUMER_H__
#include <QMouseEvent>
#include <QDialog>
#include <QPoint>

namespace Ui {
class GBSAddConsumer;
}

class GBSAddConsumer : public QDialog
{
    Q_OBJECT

public:
    explicit GBSAddConsumer(QWidget *parent = nullptr);
    ~GBSAddConsumer();

private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Ui::GBSAddConsumer *ui;
	QPoint dragPosition;
};

#endif // __GBS_ADD_CONSUMER_H__
