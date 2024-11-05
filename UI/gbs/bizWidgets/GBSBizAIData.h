#ifndef GBSBIZAIDATA_H
#define GBSBIZAIDATA_H

#include <QWidget>

namespace Ui {
class GBSBizAIData;
}

class GBSBizAIData : public QWidget {
	Q_OBJECT

public:
	explicit GBSBizAIData(QWidget *parent = nullptr);
	~GBSBizAIData();

private:
	Ui::GBSBizAIData *ui;
};

#endif // GBSBIZAIDATA_H
