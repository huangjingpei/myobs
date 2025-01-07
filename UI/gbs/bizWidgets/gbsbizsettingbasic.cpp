#include "GBSBizSettingBasic.h"
#include "ui_gbsbizsettingbasic.h"

#include <QCheckBox>
#include "gbs/common/QIniFile.h"

GBSBizSettingBasic::GBSBizSettingBasic(QSharedPointer<OBSBasicSettings> settings, QWidget *parent)
	: settings(settings),
	  QWidget(parent),
	  ui(new Ui::GBSBizSettingBasic)
{
	ui->setupUi(this);

	ui->cbxStartLive->setStyleSheet(
		"QCheckBox::indicator { image: url(:gbs/images/gbs/biz/gbs-checkbox-unchecked.png);  QCheckBox::indicator {width: 24px; height: 24px;}}"
		"QCheckBox::indicator:checked { image: url(:gbs/images/gbs/biz/gbs-checkbox-checked.png); QCheckBox::indicator {width: 24px; height: 24px;}}");
	ui->cbxStopLive->setStyleSheet(
		"QCheckBox::indicator { image: url(:gbs/images/gbs/biz/gbs-checkbox-unchecked.png);  QCheckBox::indicator {width: 24px; height: 24px;}}"
		"QCheckBox::indicator:checked { image: url(:gbs/images/gbs/biz/gbs-checkbox-checked.png); QCheckBox::indicator {width: 24px; height: 24px;}}");
	ui->cbxStopRecord->setStyleSheet(
		"QCheckBox::indicator { image: url(:gbs/images/gbs/biz/gbs-checkbox-unchecked.png);  QCheckBox::indicator {width: 24px; height: 24px;}}"
		"QCheckBox::indicator:checked { image: url(:gbs/images/gbs/biz/gbs-checkbox-checked.png); QCheckBox::indicator {width: 24px; height: 24px;}}");
	ui->cbxAutoRecord->setStyleSheet(
		"QCheckBox::indicator { image: url(:gbs/images/gbs/biz/gbs-checkbox-unchecked.png);  QCheckBox::indicator {width: 24px; height: 24px;}}"
		"QCheckBox::indicator:checked { image: url(:gbs/images/gbs/biz/gbs-checkbox-checked.png); QCheckBox::indicator {width: 24px; height: 24px;}}");

	qDebug() << "checked " << ui->cbxStartLive->isChecked();

	connect(ui->cbxStartLive, &QCheckBox::toggled, this, &GBSBizSettingBasic::onClickCBXStartLiveInd);
	connect(ui->cbxStopLive, &QCheckBox::toggled, this, &GBSBizSettingBasic::onClickCBXStopLiveInd);
	connect(ui->cbxStopRecord, &QCheckBox::toggled, this, &GBSBizSettingBasic::onClickCBXStopRecordInd);
	connect(ui->cbxAutoRecord, &QCheckBox::toggled, this, &GBSBizSettingBasic::onClickCBXAutoRecordInd);

	std::unique_ptr<IniSettings> iniFile = std::make_unique<IniSettings>("gbs.ini");
	bool enable = iniFile->value("livePusher", "startLiveConfirm", false).toBool();
	ui->cbxStartLive->setChecked(enable);
	enable = iniFile->value("livePusher", "stopLiveConfirm", false).toBool();
	ui->cbxStopLive->setChecked(enable);
	enable = iniFile->value("livePusher", "stopRecordConfirm", false).toBool();
	ui->cbxStopRecord->setChecked(enable);
	enable = iniFile->value("livePusher", "autoRecordConfirm", false).toBool();
	ui->cbxAutoRecord->setChecked(enable);

	QString qUrl = iniFile->value("livePusher", "url", "").toString();
	if (!qUrl.isEmpty()) {
		int liveIndex = qUrl.indexOf("/live/");

		// 如果找到了 "/live/"
		if (liveIndex != -1) {
			// 截取第一部分：从头到 "/live/" + 5的位置 ("/live/" 长度是5)
			QString serverUrl = qUrl.left(liveIndex + 5);

			// 截取第二部分：从 "/live/" 之后的部分
			QString key = qUrl.mid(liveIndex + 6);
			ui->lineEdit->setText(serverUrl);
			ui->lineEdit_2->setText(key);
		}
	}

	qUrl = iniFile->value("LiveBroker", "url", "").toString();
	if (!qUrl.isEmpty()) {
		ui->lineEdit_3->setText(qUrl);
	}
}

GBSBizSettingBasic::~GBSBizSettingBasic()
{
	delete ui;
}

void GBSBizSettingBasic::onClickCBXStartLiveInd(bool checked)
{

	std::unique_ptr<IniSettings> iniFile = std::make_unique<IniSettings>("gbs.ini");
	iniFile->setValue("livePusher", "startLiveConfirm", checked);

}
void GBSBizSettingBasic::onClickCBXStopLiveInd(bool checked) {
	std::unique_ptr<IniSettings> iniFile = std::make_unique<IniSettings>("gbs.ini");
	iniFile->setValue("livePusher", "stopLiveConfirm", checked);
}
void GBSBizSettingBasic::onClickCBXStopRecordInd(bool checked) {
	std::unique_ptr<IniSettings> iniFile = std::make_unique<IniSettings>("gbs.ini");
	iniFile->setValue("livePusher", "stopRecordConfirm", checked);
}
void GBSBizSettingBasic::onClickCBXAutoRecordInd(bool checked) {
	std::unique_ptr<IniSettings> iniFile = std::make_unique<IniSettings>("gbs.ini");
	iniFile->setValue("livePusher", "autoRecordConfirm", checked);
}

void GBSBizSettingBasic::SaveBasicSettings() {
	std::unique_ptr<IniSettings> iniFile = std::make_unique<IniSettings>("gbs.ini");
	iniFile->setValue("livePusher", "startLiveConfirm", ui->cbxStartLive->isChecked());
	iniFile->setValue("livePusher", "startLiveConfirm", ui->cbxStopLive->isChecked());
	iniFile->setValue("livePusher", "startLiveConfirm", ui->cbxStopRecord->isChecked());
	iniFile->setValue("livePusher", "startLiveConfirm", ui->cbxAutoRecord->isChecked());

	iniFile->setValue("livePusher", "url", ui->lineEdit->text() + "/" + ui->lineEdit_2->text());

}
