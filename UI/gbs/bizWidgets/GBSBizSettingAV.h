#ifndef GBSBIZSETTINGAV_H
#define GBSBIZSETTINGAV_H

#include <QWidget>

namespace Ui {
class GBSBizSettingAV;
}

class GBSBizSettingAV : public QWidget {
	Q_OBJECT

public:
	explicit GBSBizSettingAV(QWidget *parent = nullptr);
	~GBSBizSettingAV();

private:
	Ui::GBSBizSettingAV *ui;
};

#endif // GBSBIZSETTINGAV_H
