#ifndef GBSQRCODELOGINFORM_H
#define GBSQRCODELOGINFORM_H

#include <QWidget>

namespace Ui {
class GBSQRCodeLoginForm;
}

class GBSQRCodeLoginForm : public QWidget {
	Q_OBJECT

public:
	explicit GBSQRCodeLoginForm(QWidget *parent = nullptr);
	~GBSQRCodeLoginForm();

signals:
	void linkActivated(QString link);
	void loginTypeChanged(int type);
private slots:
	void onLinkActivated(const QString &link);
	void onLoginTypeChanged(int type);
	void onQRcodeLogin();
	void onAuthorizedLogin();
	void onLoginGBS();


private:
	Ui::GBSQRCodeLoginForm *ui;
};

#endif // GBSQRCODELOGINFORM_H
