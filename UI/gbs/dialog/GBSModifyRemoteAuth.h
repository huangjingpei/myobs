#ifndef GBSModifyRemark_H
#define GBSModifyRemark_H
#include <QMouseEvent>
#include <QDialog>
#include <QPoint>

namespace Ui {
class GBSModifyRemark;
}

class GBSModifyRemark : public QDialog
{
    Q_OBJECT

public:
    explicit GBSModifyRemark(QWidget *parent = nullptr);
    ~GBSModifyRemark();

private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Ui::GBSModifyRemark *ui;
	QPoint dragPosition;
};

#endif // GBSModifyRemark_H
