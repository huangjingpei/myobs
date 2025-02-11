#ifndef GBSQRCODELOGINFORM_H
#define GBSQRCODELOGINFORM_H

#include <QWidget>
#include "gbs/common/GBSHttpClient.h"

namespace Ui {
class GBSQRCodeLoginForm;
}

class GBSQRCodeLoginForm : public QWidget, public OBSHttpEventHandler {
	Q_OBJECT

public:
	explicit GBSQRCodeLoginForm(QWidget *parent = nullptr);
	~GBSQRCodeLoginForm();

signals:
	void linkActivated(QString link);
	void loginTypeChanged(int type);
	void onUseIconUpdate(QString iconPaht);
	void closeLoginWindow();
	void notifyLoginSuccess();
private slots:
	void onLinkActivated(const QString &link);
	void onLoginTypeChanged(int type);
	void onQRcodeLogin();
	void onAuthorizedLogin();
	void onLoginGBS();

private:
	void onMyIconDownloaded(QString path);

		private:
	Ui::GBSQRCodeLoginForm *ui;

	// 通过 OBSHttpEventHandler 继承
	void onLoginResult(const int result, const std::string token = "") override;
	void onPullRtmpUrl(const std::string url) override;
	void onUserInfo(const GBSUserInfo *info) override;
	void onUserFileDownLoad(const std::string &path, int type) override;
	void onRoomInfos(std::list<GBSRoomInfo> &info) override;
	void onRoomInfo(GBSRoomInfo *info) override;
	void onQRcodeInfo(std::string no, std::string url, int status, const std::string token = "") override;
};

#endif // GBSQRCODELOGINFORM_H
