#ifndef GBSBIZLIVEPUSHERRAUTH_H
#define GBSBIZLIVEPUSHERRAUTH_H

#include <QWidget>
#include <QKeyEvent>
#include <QList>
#include <QLineEdit>
#include "gbs/common/GBSHttpClient.h"


namespace Ui {
class GBSBizLivePusherAuth;
}

class GBSBizLivePusherAuth : public QWidget, public OBSHttpEventHandler {
	Q_OBJECT

public:
    explicit GBSBizLivePusherAuth(QWidget *parent = nullptr);
    ~GBSBizLivePusherAuth();


    void onEnterGuardCtrl(int result) override;
private slots:
    void sendEnterCtrl();

signals:
    void enterGuarderCtrl();
public:
    QString getPresetPassword();
    QString getInputPassword();
    void setPresetPassword(QString password);
private:
    void focusNextInput(int currentIndex);

private slots:
    void EnterKeyPressed();
    void DelKeyPressed();
    void ClearLineEditText(QLineEdit *editText);

private:
    Ui::GBSBizLivePusherAuth *ui;

    QList<QLineEdit*> qlists;

    QString presetPassword;    // 预设密码
    QString inputPassword;



};

#endif // GBSBIZLIVEPUSHERRAUTH_H
