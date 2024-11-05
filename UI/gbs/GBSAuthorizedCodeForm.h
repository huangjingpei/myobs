#ifndef GBSAUTHORIZEDCODEFORM_H
#define GBSAUTHORIZEDCODEFORM_H

#include <QWidget>

namespace Ui {
class GBSAuthorizedCodeForm;
}

class GBSAuthorizedCodeForm : public QWidget {
	Q_OBJECT

public:
	explicit GBSAuthorizedCodeForm(QWidget *parent = nullptr);
	~GBSAuthorizedCodeForm();

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
	Ui::GBSAuthorizedCodeForm *ui;
};

#endif // GBSAUTHORIZEDCODEFORM_H
