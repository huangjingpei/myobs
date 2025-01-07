#ifndef GBSBIZSETTINGOUTPUT_H
#define GBSBIZSETTINGOUTPUT_H

#include <QWidget>
#include <QList>
#include <QSharedPointer>
#include "window-basic-settings.hpp"
namespace Ui {
class GBSBizSettingOutput;
}

class GBSBizSettingOutput : public QWidget {
	Q_OBJECT

public:
	explicit GBSBizSettingOutput(QSharedPointer<OBSBasicSettings> settings, QWidget *parent = nullptr);
	~GBSBizSettingOutput();
	void SaveOutputSettings();

private:
	Ui::GBSBizSettingOutput *ui;

	//Too ugly. please use TabControl possibly
	QList<QWidget*> basicWidgets;
	QList<QWidget*> advancedWidgets;

	QSharedPointer<OBSBasicSettings> settings;

};

#endif // GBSBIZSETTINGOUTPUT_H
