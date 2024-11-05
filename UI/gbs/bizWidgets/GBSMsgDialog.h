#ifndef GBSMSGDIALOG_H
#define GBSMSGDIALOG_H

#include <QDialog>
#include <QString>

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
    Ui::GBSMsgDialog *ui;
};

#endif // GBSMSGDIALOG_H
