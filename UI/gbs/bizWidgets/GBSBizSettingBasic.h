#ifndef GBSBIZSETTINGBASIC_H
#define GBSBIZSETTINGBASIC_H

#include <QWidget>

namespace Ui {
class GBSBizSettingBasic;
}

class GBSBizSettingBasic : public QWidget {
	Q_OBJECT

public:
	explicit GBSBizSettingBasic(QWidget *parent = nullptr);
	~GBSBizSettingBasic();

public slots:
	void onClickCBXStartLiveInd();
	void onClickCBXStopLiveInd();
	void onClickCBXStopRecordInd();
	void onClickCBXAutoRecordInd();
private:
	Ui::GBSBizSettingBasic *ui;
};

#endif // GBSBIZSETTINGBASIC_H
