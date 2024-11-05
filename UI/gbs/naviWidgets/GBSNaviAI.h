#ifndef GBSNAVIAI_H
#define GBSNAVIAI_H

#include <QWidget>
#include <QLayout>
#include <QSharedPointer>
#include <QWeakPointer>

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


private:
	Ui::GBSNaviAI *ui;
	QWeakPointer<QLayout> weakLayoutPtr;
	QWidget* currentWidgetRef;
};

#endif // GBSNAVIAI_H
