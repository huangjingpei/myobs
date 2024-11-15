#ifndef GBSNAVIPRODUCT_H
#define GBSNAVIPRODUCT_H

#include <QWidget>
#include <QLayout>
#include <QSharedPointer>
#include <QWeakPointer>
#include <QList>
#include "gbs/common/VertNaviButton.h"
namespace Ui {
class GBSNaviProduct;
}

class GBSNaviProduct : public QWidget {
	Q_OBJECT
public:
	explicit GBSNaviProduct(QWidget *parent = nullptr);
	~GBSNaviProduct();

	void addLayoutRef(QSharedPointer<QLayout> layout, QWidget *widget);

public slots:
	void onBTTJClick();
	void onXBWJClick();
	void onSMJDClick();
	void onQCDMClick();
	void onJFSFClick();
	void onRYBHClick();
	void onMZRHClick();
	void onJXZMClick();
	void onBGWJClick();

private:
	void mariVertButton(VertNaviButton *button);
private:
	Ui::GBSNaviProduct *ui;
	QWidget* currentWidgetRef;
	QWeakPointer<QLayout> weakLayoutPtr;
	QList<VertNaviButton *> vertNaviButtons;
};

#endif // GBSNAVIPRODUCT_H
