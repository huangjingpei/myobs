#include "GBSBizAIData.h"
#include "ui_GBSBizAIData.h"

GBSBizAIData::GBSBizAIData(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GBSBizAIData)
{
	ui->setupUi(this);
}

GBSBizAIData::~GBSBizAIData()
{
	delete ui;
}
