#ifndef GBSModifyRemark_H
#define GBSModifyRemark_H
#include <QMouseEvent>
#include <QDialog>
#include <QPoint>
#include "gbs/common/GBSHttpClient.h"


namespace Ui {
class GBSModifyRemark;
}

class GBSModifyRemark : public QDialog, public OBSHttpEventHandler
{
    Q_OBJECT

public:
    explicit GBSModifyRemark(QWidget *parent = nullptr);
    ~GBSModifyRemark();

    void onModifyDevice(int result) override;

private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Ui::GBSModifyRemark *ui;
	QPoint dragPosition;
};

#endif // GBSModifyRemark_H
