#ifndef GBSBIZSETTINGLIVESOURCEDUPRM_H
#define GBSBIZSETTINGLIVESOURCEDUPRM_H

#include <QWidget>
#include <memory>
#include "gbs/common/QIniFile.h"

namespace Ui {
class GBSBizSettingLiveSourceDupRM;
}

class GBSBizSettingLiveSourceDupRM : public QWidget
{
    Q_OBJECT

public:
    explicit GBSBizSettingLiveSourceDupRM(QWidget *parent = nullptr);
    ~GBSBizSettingLiveSourceDupRM();

    void setStyleForAllSliders(QWidget *widget);
public slots:
    void onTabChanged(int index);

private:
    Ui::GBSBizSettingLiveSourceDupRM *ui;
	std::unique_ptr<IniSettings> iniFile;
};

#endif // GBSBIZSETTINGLIVESOURCEDUPRM_H
