#ifndef GBSNAVITRANSLATE_H
#define GBSNAVITRANSLATE_H

#include <QWidget>
#include <QLayout>
#include <QSharedPointer>
#include <QWeakPointer>

namespace Ui {
class GBSNaviTranslate;
}

class GBSNaviTranslate : public QWidget {
	Q_OBJECT

public:
	explicit GBSNaviTranslate(QWidget *parent = nullptr);
	~GBSNaviTranslate();

	void addLayoutRef(QSharedPointer<QLayout> layout, QWidget* widget);

public slots:
	void onBigAIClick();
	void onTextAIClick();
	void onVideoAIClick();

private:
	Ui::GBSNaviTranslate *ui;
	QWeakPointer<QLayout> weakLayoutPtr;
	QWidget* currentWidgetRef;
};

#endif // GBSNAVITRANSLATE_H
