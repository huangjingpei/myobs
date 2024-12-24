#ifndef GBSBIZLIVEGUARDERAUTH_H
#define GBSBIZLIVEGUARDERAUTH_H

#include <QWidget>
#include <QKeyEvent>
#include <QList>
#include <QLineEdit>
#include "gbs/common/GBSHttpClient.h"
namespace Ui {
class GBSBizLiveGuarderAuth;
}

class GBSBizLiveGuarderAuth : public QWidget, public OBSHttpEventHandler {
	Q_OBJECT

public:
    explicit GBSBizLiveGuarderAuth(QWidget *parent = nullptr);
    ~GBSBizLiveGuarderAuth();

signals:
    void enterGuarderCtrl();
public:
    QString getPresetPassword();
    QString getInputPassword();
    void setPresetPassword(QString password);

    void onEnterGuardCtrl(int result) override;

    private:
    void focusNextInput(int currentIndex);
    void keyPressEvent(QKeyEvent *event) override;
private:
    Ui::GBSBizLiveGuarderAuth *ui;

    QList<QLineEdit*> qlists;

    QString presetPassword;    // 预设密码
    QString inputPassword;



};

#endif // GBSBIZLIVEGUARDERAUTH_H
