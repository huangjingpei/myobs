#ifndef GBSNAVIDATA_H
#define GBSNAVIDATA_H

#include <QWidget>

#include <QLayout>
#include <QSharedPointer>
#include <QWeakPointer>
#include <QList>
#include <QTimer>
#include "gbs/common/VertNaviButton.h"
namespace Ui {
class GBSNaviData;
}

class GBSNaviData : public QWidget {
	Q_OBJECT

public:
	explicit GBSNaviData(QWidget *parent = nullptr);
	~GBSNaviData();
	void addLayoutRef(QSharedPointer<QLayout> layout, QWidget* widget);
public slots:
	void onDeviceInfoClicked();
	void onEShopDataClicked();
	void onAIDataClicked();

	void setMainBizWindow(QWidget *widget);

	void onTimeout();

	void UseIconUpdate(QString imageFile);

private:
	void mariVertButton(VertNaviButton *button);
	void seeYouNext(QString title);


private:
	Ui::GBSNaviData *ui;
	QWeakPointer<QLayout> weakLayoutPtr;
	QWidget* currentWidgetRef;
	QList<VertNaviButton *> vertNaviButtons;
	QWidget *mainWidget = nullptr;
	int timerExecCount = 0;
	QTimer *timer;
};

#endif // GBSNAVIDATA_H
