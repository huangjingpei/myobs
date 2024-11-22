#ifndef GBSBIZSETTINGOUTPUT_H
#define GBSBIZSETTINGOUTPUT_H

#include <QWidget>
#include <QList>
#include "window-basic-settings.hpp"
namespace Ui {
class GBSBizSettingOutput;
}

class GBSBizSettingOutput : public QWidget {
	Q_OBJECT

public:
	explicit GBSBizSettingOutput(OBSBasicSettings *settings, QWidget *parent = nullptr);
	~GBSBizSettingOutput();


private:
	Ui::GBSBizSettingOutput *ui;

	//Too ugly. please use TabControl possibly
	QList<QWidget*> basicWidgets;
	QList<QWidget*> advancedWidgets;

	OBSBasicSettings *settings;

};

#endif // GBSBIZSETTINGOUTPUT_H
