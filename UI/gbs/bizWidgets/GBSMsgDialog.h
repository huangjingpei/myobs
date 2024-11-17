#ifndef GBSMSGDIALOG_H
#define GBSMSGDIALOG_H

#include <QDialog>
#include <QString>
#include <QPoint>
#include <QMouseEvent>

namespace Ui {
class GBSMsgDialog;
}

class GBSMsgDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GBSMsgDialog(QString title, QLayout* lyaout = nullptr, QWidget *parent = nullptr);
    ~GBSMsgDialog();

    void resizeEvent(QResizeEvent *event) override;

private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    private:
    Ui::GBSMsgDialog *ui;
	    QPoint dragPosition;
};

#endif // GBSMSGDIALOG_H
