#ifndef GBSAddMatrix_H
#define GBSAddMatrix_H
#include <QMouseEvent>
#include <QDialog>
#include <QPoint>
#include "gbs/common/GBSHttpClient.h"

namespace Ui {
class GBSAddMatrix;
}

class GBSAddMatrix : public QDialog, public OBSHttpEventHandler
{
    Q_OBJECT

public:
    explicit GBSAddMatrix(QWidget *parent = nullptr);
    ~GBSAddMatrix();

private slots:
    void activateDevice();

    void onActivateCode(int code) override;

 private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Ui::GBSAddMatrix *ui;
	QPoint dragPosition;
};

#endif // GBSAddMatrix_H
