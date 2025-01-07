#ifndef GBSBIZSETTINGBASIC_H
#define GBSBIZSETTINGBASIC_H

#include <QWidget>
#include "window-basic-settings.hpp"
namespace Ui {
class GBSBizSettingBasic;
}

class GBSBizSettingBasic : public QWidget {
	Q_OBJECT

public:
	explicit GBSBizSettingBasic(QSharedPointer<OBSBasicSettings> settings, QWidget *parent = nullptr);
	~GBSBizSettingBasic();
	void SaveBasicSettings();

public slots:
	void onClickCBXStartLiveInd(bool checked);
	void onClickCBXStopLiveInd(bool checked);
	void onClickCBXStopRecordInd(bool checked);
	void onClickCBXAutoRecordInd(bool checked);

private:
	Ui::GBSBizSettingBasic *ui;
	QSharedPointer<OBSBasicSettings> settings;
};

#endif // GBSBIZSETTINGBASIC_H
