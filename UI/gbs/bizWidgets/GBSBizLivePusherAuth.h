#ifndef GBSBIZLIVEPUSHERRAUTH_H
#define GBSBIZLIVEPUSHERRAUTH_H

#include <QWidget>
#include <QKeyEvent>
#include <QList>
#include <QLineEdit>

namespace Ui {
class GBSBizLivePusherAuth;
}

class GBSBizLivePusherAuth : public QWidget {
	Q_OBJECT

public:
    explicit GBSBizLivePusherAuth(QWidget *parent = nullptr);
    ~GBSBizLivePusherAuth();

signals:
    void enterGuarderCtrl();
public:
    QString getPresetPassword();
    QString getInputPassword();
    void setPresetPassword(QString password);
private:
    void focusNextInput(int currentIndex);
    void keyPressEvent(QKeyEvent *event) override;
private:
    Ui::GBSBizLivePusherAuth *ui;

    QList<QLineEdit*> qlists;

    QString presetPassword;    // 预设密码
    QString inputPassword;



};

#endif // GBSBIZLIVEPUSHERRAUTH_H
