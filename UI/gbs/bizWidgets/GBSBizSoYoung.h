#ifndef GBSBIZSOYOUNG_H
#define GBSBIZSOYOUNG_H

#include <QWidget>

namespace Ui {
class GBSBizSoYoung;
}

class GBSBizSoYoung : public QWidget {
	Q_OBJECT

public:
	explicit GBSBizSoYoung(QWidget *parent = nullptr);
	~GBSBizSoYoung();

private:
	Ui::GBSBizSoYoung *ui;
};

#endif // GBSBIZSOYOUNG_H
