#include "GBSBizSettingLiveEditor.h"
#include "ui_GBSBizSettingLiveEditor.h"

GBSBizSettingLiveEditor::GBSBizSettingLiveEditor(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GBSBizSettingLiveEditor)
{
    ui->setupUi(this);
}

GBSBizSettingLiveEditor::~GBSBizSettingLiveEditor()
{
    delete ui;
}
