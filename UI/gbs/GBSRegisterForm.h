#ifndef GBSREGISTERFORM_H
#define GBSREGISTERFORM_H

#include <QWidget>

namespace Ui {
class GBSRegisterForm;
}

class GBSRegisterForm : public QWidget {
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
	Ui::GBSRegisterForm *ui;
};

#endif // GBSREGISTERFORM_H
