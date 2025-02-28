#ifndef GBSOperationReadME_H
#define GBSOperationReadME_H
#include <QMouseEvent>
#include <QDialog>
#include <QPoint>

namespace Ui {
class GBSOperationReadME;
}

class GBSOperationReadME : public QDialog
{
    Q_OBJECT

public:
    explicit GBSOperationReadME(QWidget *parent = nullptr);
    ~GBSOperationReadME();

private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Ui::GBSOperationReadME *ui;
	QPoint dragPosition;
};

#endif // GBSOperationReadME_H
