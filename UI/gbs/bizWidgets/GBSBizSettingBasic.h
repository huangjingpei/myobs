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
	explicit GBSBizSettingBasic(OBSBasicSettings *settings, QWidget *parent = nullptr);
	~GBSBizSettingBasic();

public slots:
	void onClickCBXStartLiveInd();
	void onClickCBXStopLiveInd();
	void onClickCBXStopRecordInd();
	void onClickCBXAutoRecordInd();
private:
	Ui::GBSBizSettingBasic *ui;
	OBSBasicSettings *settings;
};

#endif // GBSBIZSETTINGBASIC_H
