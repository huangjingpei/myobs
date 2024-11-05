#include "gbsnavidata.h"
#include "ui_gbsnavidata.h"
#include "../common/VertNaviButton.h"
#include "../bizWidgets/GBSBizDeviceInfo.h"
#include "../bizWidgets/GBSBizSoYoung.h"

GBSNaviData::GBSNaviData(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GBSNaviData)
{
	ui->setupUi(this);
	ui->imgTheme->setStyleSheet("border-image:url(:gbs/images/gbs/biz/gbs-theme-dark.png)");
	ui->imgAvator->setStyleSheet("border-image:url(:gbs/images/gbs/biz/gbs-logo.png)");
	QString naviTitle = R"(
	    <p style="font-size: 16px; text-align: center;">
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

	VertNaviButton* btnDeviceInfo = new VertNaviButton("数据信息", ":gbs/images/gbs/biz/gbs-discover.png", this);
	VertNaviButton* btnEShopData = new VertNaviButton("电商数据", ":gbs/images/gbs/biz/gbs-star.png", this);
	VertNaviButton* btnAIData = new VertNaviButton("智能数据", ":gbs/images/gbs/biz/gbs-timer.png", this);
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

