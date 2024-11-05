#ifndef GBSLIVEBRIDGER_H
#define GBSLIVEBRIDGER_H

#include <QWidget>

namespace Ui {
class GBSBizLiveBridger;
}

class GBSBizLiveBridger : public QWidget {
	Q_OBJECT

public:
	explicit GBSBizLiveBridger(QWidget *parent = nullptr);
	~GBSBizLiveBridger();

private:
	Ui::GBSBizLiveBridger *ui;
};

#endif // GBSLIVEBRIDGER_H
