#ifndef GBSBIZSETTINGAV_H
#define GBSBIZSETTINGAV_H

#include <QWidget>
#include "window-basic-settings.hpp"
namespace Ui {
class GBSBizSettingAV;
}

class GBSBizSettingAV : public QWidget {
	Q_OBJECT

public:
	explicit GBSBizSettingAV(OBSBasicSettings* settings, QWidget *parent = nullptr);
	~GBSBizSettingAV();



private:
	Ui::GBSBizSettingAV *ui;
	OBSBasicSettings *settings;
};

#endif // GBSBIZSETTINGAV_H
