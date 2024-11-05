#ifndef GBSMAINPROFILE_H
#define GBSMAINPROFILE_H

#include <QWidget>

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
private:
    Ui::GBSMainProfile *ui;
};

#endif // GBSMAINPROFILE_H
