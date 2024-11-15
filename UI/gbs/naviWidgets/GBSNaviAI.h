#ifndef GBSNAVIAI_H
#define GBSNAVIAI_H

#include <QWidget>
#include <QLayout>
#include <QSharedPointer>
#include <QWeakPointer>
#include <QList>
#include "gbs/common/VertNaviButton.h"
namespace Ui {
class GBSNaviAI;
}

class GBSNaviAI : public QWidget {
	Q_OBJECT

public:
	explicit GBSNaviAI(QWidget *parent = nullptr);
	~GBSNaviAI();
	void addLayoutRef(QSharedPointer<QLayout> layout, QWidget* widget);

	void onTTSlicked();
	void onFacefusionlicked();
	void onRVCClicked();
	void onLlama3Clicked();
	void onMuseTalkClicked();

    void mariVertButton(VertNaviButton* button);

private:
	Ui::GBSNaviAI *ui;
	QWeakPointer<QLayout> weakLayoutPtr;
	QWidget* currentWidgetRef;

    QList<VertNaviButton*> vertNaviButtons;
};

#endif // GBSNAVIAI_H
