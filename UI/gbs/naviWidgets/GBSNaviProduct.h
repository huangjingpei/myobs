#ifndef GBSNAVIPRODUCT_H
#define GBSNAVIPRODUCT_H

#include <QWidget>
#include <QLayout>
#include <QSharedPointer>
#include <QWeakPointer>
namespace Ui {
class GBSNaviProduct;
}

class GBSNaviProduct : public QWidget {
	Q_OBJECT

public:
	explicit GBSNaviProduct(QWidget *parent = nullptr);
	~GBSNaviProduct();
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
	Ui::GBSNaviProduct *ui;
	QWidget* currentWidgetRef;
	QWeakPointer<QLayout> weakLayoutPtr;
};

#endif // GBSNAVIPRODUCT_H
