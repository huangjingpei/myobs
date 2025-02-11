#ifndef GBSNORMALLOGINFORM_H
#define GBSNORMALLOGINFORM_H

#include <QWidget>
#include <memory>
#include "gbs/common/GBSHttpClient.h"
#include "window-main.hpp"
#include <QPoint>
#include "gbs/common/XORPasswordProtecter.h"
namespace Ui {
class GBSNormalLoginForm;
}

class GBSNormalLoginForm : public OBSMainWindow, public OBSHttpEventHandler {
	Q_OBJECT

public:
	explicit GBSNormalLoginForm(QWidget *parent = nullptr);
	~GBSNormalLoginForm();
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
	void onPasswordForgot();
	void onLoginGBS();


private:
	Ui::GBSNormalLoginForm *ui;


// 通过 OBSHttpEventHandler 继承
	void onLoginResult(const int result, const std::string token = "") override;
	void onPullRtmpUrl(const std::string url) override;
	void onUserInfo(const GBSUserInfo *info) override;
	void onUserFileDownLoad(const std::string &path, int type) override;
	void onRoomInfos(std::list<GBSRoomInfo> &info) override;
	void onRoomInfo(GBSRoomInfo *info) override;
	void onAgreementInfo(std::string richText, int type) override;

	// 通过 OBSMainWindow 继承
	config_t *Config() const override;
	void OBSInit() override;
	int GetProfilePath(char *path, size_t size, const char *file) const override;

	std::unique_ptr<XORPasswordProtecter> passwordProtecter;

	
};

#endif // GBSNORMALLOGINFORM_H
