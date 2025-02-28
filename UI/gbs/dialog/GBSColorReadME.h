#ifndef GBSColorReadME_H
#define GBSColorReadME_H
#include <QMouseEvent>
#include <QDialog>
#include <QPoint>

namespace Ui {
class GBSColorReadME;
}

class GBSColorReadME : public QDialog
{
    Q_OBJECT

public:
    explicit GBSColorReadME(QWidget *parent = nullptr);
    ~GBSColorReadME();

private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Ui::GBSColorReadME *ui;
	QPoint dragPosition;
};

#endif // GBSColorReadME_H
