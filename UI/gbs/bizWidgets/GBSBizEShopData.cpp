#include "gbsbizeshopdata.h"
#include "ui_gbsbizeshopdata.h"

GBSBizEShopData::GBSBizEShopData(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GBSBizEShopData)
{
	ui->setupUi(this);
}

GBSBizEShopData::~GBSBizEShopData()
{
	delete ui;
}
