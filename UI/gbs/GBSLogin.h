#ifndef GBSLOGIN_H
#define GBSLOGIN_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class GBSLogin;
}
QT_END_NAMESPACE

class GBSLogin : public QWidget {
	Q_OBJECT

public:
	GBSLogin(QWidget *parent = nullptr);
	~GBSLogin();

private:
	Ui::GBSLogin *ui;
};
#endif // GBSLOGIN_H
