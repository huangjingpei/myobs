#ifndef GBSMAINPROFILE_H
#define GBSMAINPROFILE_H

#include <QWidget>
#include <QTimer>
namespace Ui {
class GBSMainProfile;
}



class GBSMainProfile : public QWidget
{
    Q_OBJECT

public:
    explicit GBSMainProfile(QWidget *parent = nullptr);
    ~GBSMainProfile();

public slots:
    void exitSystemAndGoLogin(bool checked);
	void onShowSetting();
    void resetClickCount();

private:
    Ui::GBSMainProfile *ui;
	QTimer *timer;
	int clickCount = 0;
};

#endif // GBSMAINPROFILE_H
