#ifndef GBSAUTHORIZEDCODEFORM_H
#define GBSAUTHORIZEDCODEFORM_H

#include <QWidget>
#include "gbs/dto/GBSUserInfo.h"
#include "gbs/common/GBSHttpClient.h"
#include "gbs/common/XORPasswordProtecter.h"


namespace Ui {
class GBSAuthorizedCodeForm;
}

class GBSAuthorizedCodeForm : public QWidget, public OBSHttpEventHandler {
	Q_OBJECT

public:
	explicit GBSAuthorizedCodeForm(QWidget *parent = nullptr);
	~GBSAuthorizedCodeForm();

signals:
	void linkActivated(QString link);
	void loginTypeChanged(int type);
	void closeLoginWindow();
	void notifyLoginSuccess();
private slots:
	void onLinkActivated(const QString &link);
	void onLoginTypeChanged(int type);
	void onQRcodeLogin();
	void onAuthorizedLogin();
	void onLoginGBS();

private:
	void onUserInfo(const GBSUserInfo *info);
	void onLoginResult(const int result, const std::string token);

	std::unique_ptr<XORPasswordProtecter> passwordProtecter;

private:
	Ui::GBSAuthorizedCodeForm *ui;
};

#endif // GBSAUTHORIZEDCODEFORM_H
