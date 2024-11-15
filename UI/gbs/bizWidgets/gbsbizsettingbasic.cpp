#include "GBSBizSettingBasic.h"
#include "ui_gbsbizsettingbasic.h"

#include <QCheckBox>

GBSBizSettingBasic::GBSBizSettingBasic(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GBSBizSettingBasic)
{
	ui->setupUi(this);

	ui->cbxStartLive->setStyleSheet("QCheckBox::indicator { image: url(:gbs/images/gbs/biz/gbs-checkbox-unchecked.png);  QCheckBox::indicator {width: 24px; height: 24px;}}"
					"QCheckBox::indicator:checked { image: url(:gbs/images/gbs/biz/gbs-checkbox-checked.png); QCheckBox::indicator {width: 24px; height: 24px;}}");
	ui->cbxStopLive->setStyleSheet("QCheckBox::indicator { image: url(:gbs/images/gbs/biz/gbs-checkbox-unchecked.png);  QCheckBox::indicator {width: 24px; height: 24px;}}"
				       "QCheckBox::indicator:checked { image: url(:gbs/images/gbs/biz/gbs-checkbox-checked.png); QCheckBox::indicator {width: 24px; height: 24px;}}");
	ui->cbxStopRecord->setStyleSheet("QCheckBox::indicator { image: url(:gbs/images/gbs/biz/gbs-checkbox-unchecked.png);  QCheckBox::indicator {width: 24px; height: 24px;}}"
					"QCheckBox::indicator:checked { image: url(:gbs/images/gbs/biz/gbs-checkbox-checked.png); QCheckBox::indicator {width: 24px; height: 24px;}}");
	ui->cbxAutoRecord->setStyleSheet("QCheckBox::indicator { image: url(:gbs/images/gbs/biz/gbs-checkbox-unchecked.png);  QCheckBox::indicator {width: 24px; height: 24px;}}"
					"QCheckBox::indicator:checked { image: url(:gbs/images/gbs/biz/gbs-checkbox-checked.png); QCheckBox::indicator {width: 24px; height: 24px;}}");


	qDebug()<< "checked " << ui->cbxStartLive->isChecked();

	connect(ui->cbxStartLive, &QCheckBox::clicked, this, &GBSBizSettingBasic::onClickCBXStartLiveInd);
	connect(ui->cbxStopLive, &QCheckBox::clicked, this, &GBSBizSettingBasic::onClickCBXStopLiveInd);
	connect(ui->cbxStopRecord, &QCheckBox::clicked, this, &GBSBizSettingBasic::onClickCBXStopRecordInd);
	connect(ui->cbxAutoRecord, &QCheckBox::clicked, this, &GBSBizSettingBasic::onClickCBXAutoRecordInd);

}

GBSBizSettingBasic::~GBSBizSettingBasic()
{
	delete ui;
}

void GBSBizSettingBasic::onClickCBXStartLiveInd() {
	//TODO 保存文件
}
void GBSBizSettingBasic::onClickCBXStopLiveInd() {

}
void GBSBizSettingBasic::onClickCBXStopRecordInd() {

}
void GBSBizSettingBasic::onClickCBXAutoRecordInd() {

}
