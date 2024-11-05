#ifndef GBSBIZESHOPDATA_H
#define GBSBIZESHOPDATA_H

#include <QWidget>

namespace Ui {
class GBSBizEShopData;
}

class GBSBizEShopData : public QWidget {
	Q_OBJECT

public:
	explicit GBSBizEShopData(QWidget *parent = nullptr);
	~GBSBizEShopData();

private:
	Ui::GBSBizEShopData *ui;
};

#endif // GBSBIZESHOPDATA_H
