#ifndef GBSREGISTERFORM_H
#define GBSREGISTERFORM_H

#include <QWidget>
#include "gbs/common/GBSHttpClient.h"

namespace Ui {
class GBSRegisterForm;
}

class GBSRegisterForm : public QWidget, public OBSHttpEventHandler {
	Q_OBJECT

public:
	explicit GBSRegisterForm(QWidget *parent = nullptr);
	~GBSRegisterForm();

signals:
	void linkActivated(QString link);
	void loginTypeChanged(int type);
private slots:
	void onLinkActivated(const QString &link);
	void onLoginTypeChanged(int type);
	void onQRcodeLogin();
	void onAuthorizedLogin();

private:

	void onAgreementInfo(std::string richText, int type) override;

private:
	Ui::GBSRegisterForm *ui;
};

#endif // GBSREGISTERFORM_H
