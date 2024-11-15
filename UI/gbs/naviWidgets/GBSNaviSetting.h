#ifndef GBSNAVISETTING_H
#define GBSNAVISETTING_H

#include <QWidget>
#include <QLayout>
#include <QSharedPointer>
#include <QWeakPointer>
#include <QList>
#include "gbs/common/VertNaviButton.h"
namespace Ui {
class GBSNaviSetting;
}

class GBSNaviSetting : public QWidget {
	Q_OBJECT

public:
	explicit GBSNaviSetting(QWidget *parent = nullptr);
	~GBSNaviSetting();
	void addLayoutRef(QSharedPointer<QLayout> layout, QWidget* widget);

public slots:
	void onBaiscClick();
	void onOutputClick();
	void onAVClick();

	void onProductDupRMClick();
	void onTimbreSquareClick();
	void onBridgerMgrClick();
	void onLiveEditorClick();
	void onLiveSourcDupRMClick();

private:
    void mariVertButton(VertNaviButton *button);

private:
	Ui::GBSNaviSetting *ui;
	QWidget* currentWidgetRef;
	QWeakPointer<QLayout> weakLayoutPtr;

	QList<VertNaviButton *> vertNaviButtons;

};

#endif // GBSNAVISETTING_H
