#include "GBSBizSettingPage.h"
#include "ui_GBSBizSettingPage.h"

#include "gbs/bizWidgets/GBSBizSettingAV.h"
#include "gbs/bizWidgets/GBSBizSettingBasic.h"
#include "gbs/bizWidgets/GBSBizSettingOutput.h"
#include "window-basic-main.hpp"


GBSBizSettingPage::GBSBizSettingPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GBSBizSettingPage)
{
    ui->setupUi(this);

    connect(ui->pushButton_12, &QPushButton::clicked, this, &GBSBizSettingPage::onClickBasic);
    connect(ui->pushButton_13, &QPushButton::clicked, this, &GBSBizSettingPage::onClickAVOutput);
    connect(ui->pushButton_14, &QPushButton::clicked, this, &GBSBizSettingPage::onClickAVSetting);
    settings = new OBSBasicSettings(OBSBasic::Get());
    GBSBizSettingOutput *avSetting = new GBSBizSettingOutput(settings);
    ui->horizontalLayout->addWidget(avSetting);
    currentWidget = avSetting;
    connect(ui->btnConfirm, &QPushButton::clicked, this, &GBSBizSettingPage::onOK);
    connect(ui->btnCancel, &QPushButton::clicked, this, &GBSBizSettingPage::onCancel);
    connect(ui->btnApply, &QPushButton::clicked, this, &GBSBizSettingPage::onApply);

    buttons << ui->pushButton_12 << ui->pushButton_13 << ui->pushButton_14;


    iconPathes << ":gbs/images/gbs/biz/gbs-setting-basic.png"
	       << ":gbs/images/gbs/biz/gbs-setting-output.png"
	       << ":gbs/images/gbs/biz/gbs-setting-av.png";

    markButton(ui->pushButton_13);



}

GBSBizSettingPage::~GBSBizSettingPage()
{
    delete ui;
}

void GBSBizSettingPage::markButton(QPushButton *button)
{

	for (int i = 0; i < buttons.count(); i++) {
		QPushButton *it = buttons.at(i);
		QString iconPath = iconPathes.at(i);
		if (it == button) {
			int lastDotIndex = iconPath.lastIndexOf('.');
			QString fileNameWithoutExtension = iconPath.left(lastDotIndex);
			QString fileExtension = iconPath.mid(lastDotIndex);
			QString fileName = fileNameWithoutExtension + "-blue" + fileExtension;
			QPixmap pixmap(fileName);
			QPixmap scaledPixmap = pixmap.scaled(25, 25, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			it->setIcon(QIcon(scaledPixmap));
			it->setIconSize(QSize(25, 25));
			it->setStyleSheet("QPushButton { color: #00C566; }");

		} else {
			QPixmap pixmap(iconPath);
			QPixmap scaledPixmap = pixmap.scaled(25, 25, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			it->setIcon(QIcon(scaledPixmap));
			it->setIconSize(QSize(25, 25));
			it->setStyleSheet("QPushButton { color: #78828A; }");

		}
	}
}

void GBSBizSettingPage::onOK() {
	settings->onCofirm();
}
void GBSBizSettingPage::onCancel() {
	settings->onCancel();
}
void GBSBizSettingPage::onApply() {
	settings->onApply();
	GBSBizSettingBasic *gbsBizSettingBasic = qobject_cast<GBSBizSettingBasic *>(currentWidget);
	if (gbsBizSettingBasic != nullptr) {
		gbsBizSettingBasic->SaveBasicSettings();
	}

	GBSBizSettingOutput *gbsBizSettingOutput = qobject_cast<GBSBizSettingOutput *>(currentWidget);
	if (gbsBizSettingOutput != nullptr) {
		gbsBizSettingOutput->SaveOutputSettings();
	}

	GBSBizSettingAV *gbsBizSettingAV = qobject_cast<GBSBizSettingAV *>(currentWidget);
	if (gbsBizSettingAV != nullptr) {
		gbsBizSettingAV->SaveAudioSettings();
		gbsBizSettingAV->SaveVideoSettings();
	}
	


}

void GBSBizSettingPage::onClickBasic()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    markButton(button);
    QWidget *widget = qobject_cast<GBSBizSettingBasic *>(currentWidget);
    if (widget == nullptr) {
        ui->horizontalLayout->removeWidget(currentWidget);
        delete currentWidget;
        currentWidget = nullptr;
	GBSBizSettingBasic *avBasic = new GBSBizSettingBasic(settings);
        ui->horizontalLayout->addWidget(avBasic);
        currentWidget = avBasic;
    }

}
void GBSBizSettingPage::onClickAVOutput() {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    markButton(button);
    QWidget *widget = qobject_cast<GBSBizSettingOutput *>(currentWidget);
    if (widget == nullptr) {
        ui->horizontalLayout->removeWidget(currentWidget);
        delete currentWidget;
        currentWidget = nullptr;
	GBSBizSettingOutput *avOuput = new GBSBizSettingOutput(settings);
        ui->horizontalLayout->addWidget(avOuput);
        currentWidget = avOuput;
    }
}
void GBSBizSettingPage::onClickAVSetting() {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    markButton(button);
    QWidget *widget = qobject_cast<GBSBizSettingAV *>(currentWidget);
    if (widget == nullptr) {
        ui->horizontalLayout->removeWidget(currentWidget);
        delete currentWidget;
        currentWidget = nullptr;
	GBSBizSettingAV *avWidget = new GBSBizSettingAV(settings);
        ui->horizontalLayout->addWidget(avWidget);
        currentWidget = avWidget;
    }
}


