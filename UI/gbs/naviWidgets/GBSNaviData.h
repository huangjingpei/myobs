#ifndef GBSNAVIDATA_H
#define GBSNAVIDATA_H

#include <QWidget>

#include <QLayout>
#include <QSharedPointer>
#include <QWeakPointer>

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

	void onMyIconDownloaded(QString path);

private:
	Ui::GBSNaviData *ui;
	QWeakPointer<QLayout> weakLayoutPtr;
	QWidget* currentWidgetRef;
};

#endif // GBSNAVIDATA_H
