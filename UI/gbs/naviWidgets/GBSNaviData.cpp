#include "gbsnavidata.h"
#include "ui_gbsnavidata.h"
#include "../common/VertNaviButton.h"
#include "../bizWidgets/GBSBizDeviceInfo.h"
#include "../bizWidgets/GBSBizSoYoung.h"
#include "window-basic-main.hpp"

GBSNaviData::GBSNaviData(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GBSNaviData)
{
	ui->setupUi(this);
	ui->imgTheme->setStyleSheet("border-image:url(:gbs/images/gbs/biz/gbs-theme-dark.png)");
	QString naviTitle = R"(
	    <p style="font-size: 16px; text-align: left;">
		<span style="color: #9CA4AB;">数据大屏</span>
	    </p>
	)";

	ui->lblNaviTitle->setText(naviTitle);


        QString welcomeMessage = R"(
            <p style="font-size: 12px; text-align: center;">
                <span style="color: black;">Hi！</span>
                <span style="color: black;">尊贵的用户</span>
            </p>
        )";
        ui->lblHelloMessage->setText(welcomeMessage);

        QString nickName = R"(
            <p style="font-size: 10px; text-align: center;">
                <span style="color: black;">hjp9221@163.com</span>
            </p>
        )";



	ui->lblNickName->setText(nickName);

	VertNaviButton* btnDeviceInfo = new VertNaviButton("数据信息", ":gbs/images/gbs/biz/gbs-data-device-info.png", this);
	VertNaviButton* btnEShopData = new VertNaviButton("电商数据", ":gbs/images/gbs/biz/gbs-data-running-data.png", this);
	VertNaviButton* btnAIData = new VertNaviButton("智能数据", ":gbs/images/gbs/biz/gbs-data-matrix-screen.png", this);
	btnDeviceInfo->setFixedSize(205, 40);
	btnEShopData->setFixedSize(205, 40);
	btnAIData->setFixedSize(205, 40);
	ui->verticalLayout->addWidget(btnDeviceInfo);
	ui->verticalLayout->addWidget(btnEShopData);
	ui->verticalLayout->addWidget(btnAIData);
	QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	ui->verticalLayout->addSpacerItem(verticalSpacer);

	connect(btnDeviceInfo, &QPushButton::clicked, this, &GBSNaviData::onDeviceInfoClicked);
	connect(btnEShopData, &QPushButton::clicked, this,
		&GBSNaviData::onEShopDataClicked);
	connect(btnAIData, &QPushButton::clicked, this,
		&GBSNaviData::onAIDataClicked);
	OBSBasic *main = OBSBasic::Get();

	connect(main, &OBSBasic::onUseIconUpdate, this, &GBSNaviData::onMyIconDownloaded);




}

GBSNaviData::~GBSNaviData()
{
	delete ui;
}


void GBSNaviData::addLayoutRef(QSharedPointer<QLayout> layout, QWidget* widget) {
	weakLayoutPtr = layout;
	currentWidgetRef = widget;
	qDebug() << "addLayoutRef debug layout ptr " << layout.data();
}

void GBSNaviData::onDeviceInfoClicked() {
	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {

		layout->removeWidget(currentWidgetRef);
		delete currentWidgetRef;
		GBSBizDeviceInfo* gbsBizDeviceInfo = new GBSBizDeviceInfo(this);
		layout->addWidget(gbsBizDeviceInfo);
		currentWidgetRef = gbsBizDeviceInfo;



	}
}

void GBSNaviData::onEShopDataClicked() {
	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {

		layout->removeWidget(currentWidgetRef);
		delete currentWidgetRef;
		GBSBizSoYoung* gbsBizSoYoung = new GBSBizSoYoung(this);
		layout->addWidget(gbsBizSoYoung);
		currentWidgetRef = gbsBizSoYoung;



	}
}


void GBSNaviData::onAIDataClicked() {
	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {

		layout->removeWidget(currentWidgetRef);
		delete currentWidgetRef;
		GBSBizSoYoung* gbsBizSoYoung = new GBSBizSoYoung(this);
		layout->addWidget(gbsBizSoYoung);
		currentWidgetRef = gbsBizSoYoung;



	}
}


void GBSNaviData::onMyIconDownloaded(QString path) {

	QPixmap pixmap(path);
	ui->imgAvator->setPixmap(pixmap.scaled(48, 48, Qt::KeepAspectRatio));
	ui->imgAvator->setStyleSheet(
		" border-radius: 50%;");
}
