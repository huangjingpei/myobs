#ifndef GBSBIZSETTINGOUTPUT_H
#define GBSBIZSETTINGOUTPUT_H

#include <QWidget>
#include <QList>
namespace Ui {
class GBSBizSettingOutput;
}

class GBSBizSettingOutput : public QWidget {
	Q_OBJECT

public:
	explicit GBSBizSettingOutput(QWidget *parent = nullptr);
	~GBSBizSettingOutput();


private:
	Ui::GBSBizSettingOutput *ui;

	//Too ugly. please use TabControl possibly
	QList<QWidget*> basicWidgets;
	QList<QWidget*> advancedWidgets;

};

#endif // GBSBIZSETTINGOUTPUT_H
