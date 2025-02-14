#ifndef GBSREMARKSLICES_H
#define GBSREMARKSLICES_H
#include <QMouseEvent>
#include <QDialog>
#include <QPoint>

namespace Ui {
class GBSRemakeSlices;
}

class GBSRemakeSlices : public QDialog
{
    Q_OBJECT

public:
    explicit GBSRemakeSlices(QWidget *parent = nullptr);
    ~GBSRemakeSlices();

private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Ui::GBSRemakeSlices *ui;
	QPoint dragPosition;
};

#endif // GBSREMARKSLICES_H
