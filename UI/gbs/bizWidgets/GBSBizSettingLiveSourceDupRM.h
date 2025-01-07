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
	void onApply(bool checked = false);
    void onCancel(bool checked = false);
	void onConfirm(bool checked = false);

private:
    Ui::GBSBizSettingLiveSourceDupRM *ui;
	std::unique_ptr<IniSettings> iniFile;

    private:
	QString mImageDupRm{""};
	QString mOtherPlatDmpRm{""};
	QString mTimerClockDupRm{""};
	QString mWhosCommingDupRm{""};
	QString mAudioEffectDupRm{""};
};

#endif // GBSBIZSETTINGLIVESOURCEDUPRM_H
