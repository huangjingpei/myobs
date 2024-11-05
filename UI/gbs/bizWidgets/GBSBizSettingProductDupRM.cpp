#include "GBSBizSettingProductDupRM.h"
#include "ui_GBSBizSettingProductDupRM.h"

GBSBizSettingProductDupRM::GBSBizSettingProductDupRM(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GBSBizSettingProductDupRM)
{
    ui->setupUi(this);
}

GBSBizSettingProductDupRM::~GBSBizSettingProductDupRM()
{
    delete ui;
}
