#include "gbsnavisetting.h"
#include "ui_gbsnavisetting.h"

#include "../common/VertNaviButton.h"
#include "../bizWidgets/GBSBizSettingBasic.h"
#include "../bizWidgets/GBSBizSettingOutput.h"
#include "../bizWidgets/GBSBizSettingAV.h"
#include "../bizWidgets/GBSBizSettingProductDupRM.h"
#include "../bizWidgets/GBSBizSettingTimbreSquare.h"
#include "../bizWidgets/GBSBizSettingBridgerMgr.h"
#include "../bizWidgets/GBSBizSettingLiveEditor.h"
#include "../bizWidgets/GBSBizSettingLiveSourceDupRM.h"

GBSNaviSetting::GBSNaviSetting(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GBSNaviSetting)
{
	ui->setupUi(this);
	ui->imgTheme->setStyleSheet("border-image:url(:gbs/images/gbs/biz/gbs-theme-dark.png)");
	ui->imgAvator->setStyleSheet("border-image:url(:gbs/images/gbs/biz/gbs-logo.png)");
	QString naviTitle = R"(
	    <p style="font-size: 16px; text-align: center;">
		<span style="color: #9CA4AB;">直播设置</span>
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

	VertNaviButton *btnBasic = new VertNaviButton("基础设置", ":gbs/images/gbs/biz/gbs-setting-basic.png", this);
	VertNaviButton *btnProductDupRM =
		new VertNaviButton("商品素材去重", ":gbs/images/gbs/biz/gbs-setting-product-material.png", this);
	VertNaviButton *btnTimbreSquare =
		new VertNaviButton("直播音色广场", ":gbs/images/gbs/biz/gbs-setting-timbre-square.png", this);
	VertNaviButton *btnBridgerMgr =
		new VertNaviButton("代播号管理", ":gbs/images/gbs/biz/gbs-setting-bridger-mgr.png", this);
	VertNaviButton *btnLiveEditor =
		new VertNaviButton("主播排版管理", ":gbs/images/gbs/biz/gbs-setting-live-edit.png", this);
	VertNaviButton *btnLiveSourcDupRM =
		new VertNaviButton("源直播去重", ":gbs/images/gbs/biz/gbs-setting-source-dumrm.png", this);

	btnBasic->setFixedSize(205, 40);
	btnProductDupRM->setFixedSize(205, 40);
	btnTimbreSquare->setFixedSize(205, 40);
	btnBridgerMgr->setFixedSize(205, 40);
	btnLiveEditor->setFixedSize(205, 40);
	btnLiveSourcDupRM->setFixedSize(205, 40);

	ui->verticalLayout->addWidget(btnBasic);
	ui->verticalLayout->addWidget(btnProductDupRM);
	ui->verticalLayout->addWidget(btnTimbreSquare);
	ui->verticalLayout->addWidget(btnBridgerMgr);
	ui->verticalLayout->addWidget(btnLiveEditor);
	ui->verticalLayout->addWidget(btnLiveSourcDupRM);

	QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	ui->verticalLayout->addSpacerItem(verticalSpacer);

	ui->lblNickName->setText(nickName);

	connect(btnBasic, &QPushButton::clicked, this, &GBSNaviSetting::onBaiscClick);
	connect(btnProductDupRM, &QPushButton::clicked, this, &GBSNaviSetting::onProductDupRMClick);
	connect(btnTimbreSquare, &QPushButton::clicked, this, &GBSNaviSetting::onTimbreSquareClick);
	connect(btnBridgerMgr, &QPushButton::clicked, this, &GBSNaviSetting::onBridgerMgrClick);
	connect(btnLiveEditor, &QPushButton::clicked, this, &GBSNaviSetting::onLiveEditorClick);
	connect(btnLiveSourcDupRM, &QPushButton::clicked, this, &GBSNaviSetting::onLiveSourcDupRMClick);



}

GBSNaviSetting::~GBSNaviSetting()
{
	delete ui;
}


void GBSNaviSetting::addLayoutRef(QSharedPointer<QLayout> layout, QWidget *widget)
{
	weakLayoutPtr = layout;
	currentWidgetRef = widget;
}

void GBSNaviSetting::onBaiscClick()
{

	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {
		//这里是否可以使用replaceWidget ???
		layout->removeWidget(currentWidgetRef);
		delete currentWidgetRef;
		GBSBizSettingBasic *gbsBizSettingBasic = new GBSBizSettingBasic(this);
		layout->addWidget(gbsBizSettingBasic);
		currentWidgetRef = gbsBizSettingBasic;
	}
}

void GBSNaviSetting::onProductDupRMClick()
{
	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {

		layout->removeWidget(currentWidgetRef);
		delete currentWidgetRef;

		GBSBizSettingProductDupRM *gbsBizSettingProductDupRM = new GBSBizSettingProductDupRM(this);
		layout->addWidget(gbsBizSettingProductDupRM);
		currentWidgetRef = gbsBizSettingProductDupRM;
	}
}

void GBSNaviSetting::onTimbreSquareClick()
{
	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {

		layout->removeWidget(currentWidgetRef);
		delete currentWidgetRef;

		GBSBizSettingTimbreSquare *gbsBizSettingTimbreSquare = new GBSBizSettingTimbreSquare(this);
		layout->addWidget(gbsBizSettingTimbreSquare);
		currentWidgetRef = gbsBizSettingTimbreSquare;
	}
}

void GBSNaviSetting::onBridgerMgrClick()
{
	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {

		layout->removeWidget(currentWidgetRef);
		delete currentWidgetRef;

		GBSBizSettingBridgerMgr *gbsBizSettingBridgerMgr = new GBSBizSettingBridgerMgr(this);
		layout->addWidget(gbsBizSettingBridgerMgr);
		currentWidgetRef = gbsBizSettingBridgerMgr;
	}
}

void GBSNaviSetting::onLiveEditorClick()
{
	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {

		layout->removeWidget(currentWidgetRef);
		delete currentWidgetRef;

		GBSBizSettingLiveEditor *gbsBizSettingLiveEditor = new GBSBizSettingLiveEditor(this);
		layout->addWidget(gbsBizSettingLiveEditor);
		currentWidgetRef = gbsBizSettingLiveEditor;
	}
}

void GBSNaviSetting::onLiveSourcDupRMClick()
{
	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {

		layout->removeWidget(currentWidgetRef);
		delete currentWidgetRef;

		GBSBizSettingLiveSourceDupRM *gbsBizSettingLiveSourceDupRM = new GBSBizSettingLiveSourceDupRM(this);
		layout->addWidget(gbsBizSettingLiveSourceDupRM);
		currentWidgetRef = gbsBizSettingLiveSourceDupRM;
	}
}

void GBSNaviSetting::onOutputClick()
{
	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {


		layout->removeWidget(currentWidgetRef);
		delete currentWidgetRef;

		GBSBizSettingOutput* gbsBizSettingOutput = new GBSBizSettingOutput(this);
		layout->addWidget(gbsBizSettingOutput);
		currentWidgetRef = gbsBizSettingOutput;

	}
}
void GBSNaviSetting::onAVClick() {
	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {

		layout->removeWidget(currentWidgetRef);
		delete currentWidgetRef;
		GBSBizSettingAV* gbsBizSettingAV = new GBSBizSettingAV(this);
		layout->addWidget(gbsBizSettingAV);
		currentWidgetRef = gbsBizSettingAV;



	}
}
