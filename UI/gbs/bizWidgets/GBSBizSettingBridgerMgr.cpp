#include "GBSBizSettingBridgerMgr.h"
#include "ui_GBSBizSettingBridgerMgr.h"

GBSBizSettingBridgerMgr::GBSBizSettingBridgerMgr(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GBSBizSettingBridgerMgr)
{
    ui->setupUi(this);
}

GBSBizSettingBridgerMgr::~GBSBizSettingBridgerMgr()
{
    delete ui;
}
