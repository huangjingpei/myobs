#include "GBSBizSettingPage.h"
#include "ui_GBSBizSettingPage.h"

#include "gbs/bizWidgets/GBSBizSettingAV.h"
#include "gbs/bizWidgets/GBSBizSettingBasic.h"
#include "gbs/bizWidgets/GBSBizSettingOutput.h"

GBSBizSettingPage::GBSBizSettingPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GBSBizSettingPage)
{
    ui->setupUi(this);
    connect(ui->pushButton_12, &QPushButton::clicked, this, &GBSBizSettingPage::onClickBasic);
    connect(ui->pushButton_13, &QPushButton::clicked, this, &GBSBizSettingPage::onClickAVOutput);
    connect(ui->pushButton_14, &QPushButton::clicked, this, &GBSBizSettingPage::onClickAVSetting);
    GBSBizSettingOutput *avSetting = new GBSBizSettingOutput();
    ui->horizontalLayout->addWidget(avSetting);
    currentWidget = avSetting;


}

GBSBizSettingPage::~GBSBizSettingPage()
{
    delete ui;
}


void GBSBizSettingPage::onClickBasic() {
    QWidget *widget = qobject_cast<GBSBizSettingBasic *>(currentWidget);
    if (widget == nullptr) {
        ui->horizontalLayout->removeWidget(currentWidget);
        delete currentWidget;
        currentWidget = nullptr;
        GBSBizSettingBasic *avBasic = new GBSBizSettingBasic();
        ui->horizontalLayout->addWidget(avBasic);
        currentWidget = avBasic;
    }

}
void GBSBizSettingPage::onClickAVOutput() {
    QWidget *widget = qobject_cast<GBSBizSettingOutput *>(currentWidget);
    if (widget == nullptr) {
        ui->horizontalLayout->removeWidget(currentWidget);
        delete currentWidget;
        currentWidget = nullptr;
        GBSBizSettingOutput *avOuput = new GBSBizSettingOutput();
        ui->horizontalLayout->addWidget(avOuput);
        currentWidget = avOuput;
    }
}
void GBSBizSettingPage::onClickAVSetting() {
    QWidget *widget = qobject_cast<GBSBizSettingAV *>(currentWidget);
    if (widget == nullptr) {
        ui->horizontalLayout->removeWidget(currentWidget);
        delete currentWidget;
        currentWidget = nullptr;
        GBSBizSettingAV *avWidget = new GBSBizSettingAV();
        ui->horizontalLayout->addWidget(avWidget);
        currentWidget = avWidget;
    }
}
