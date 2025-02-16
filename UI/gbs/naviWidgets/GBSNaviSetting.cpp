#include "gbsnavisetting.h"
#include "ui_gbsnavisetting.h"

#include "../common/VertNaviButton.h"
#include "../bizWidgets/GBSBizSettingPage.h"
#include "../bizWidgets/GBSBizSettingOutput.h"
#include "../bizWidgets/GBSBizSettingAV.h"
#include "../bizWidgets/GBSBizSettingProductDupRM.h"
#include "../bizWidgets/GBSBizSettingTimbreSquare.h"
#include "../bizWidgets/GBSBizSettingBridgerMgr.h"
#include "../bizWidgets/GBSBizSettingLiveEditor.h"
#include "../bizWidgets/GBSBizSettingLiveSourceDupRM.h"
#include "window-basic-main.hpp"
#include "gbs/GBSMainCollector.h"
#include "gbs/dto/GBSLiveAccountInfo.h"

GBSNaviSetting::GBSNaviSetting(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GBSNaviSetting)
{
	ui->setupUi(this);
	ui->imgTheme->setStyleSheet("border-image:url(:gbs/images/gbs/biz/gbs-theme-dark.png)");
	ui->imgTheme->setVisible(false);
	ui->radioButton->setVisible(false);
	ui->lblNaviTitle->setStyleSheet("QLabel {"
					"    background: transparent;"
					"    color: #1B2846;"     // 文本颜色
					"    font-size: 14px;"    // 字体大小
					"    padding-left: 15px;" // 左对齐并添加15px的内边距
					"}");

	ui->lblNaviTitle->setText("直播设置");



        QString welcomeMessage = R"(
            <p style="font-size: 12px; text-align: center;">
                <span style="color: black;">Hi！</span>
                <span style="color: black;">尊贵的用户</span>
            </p>
        )";
        ui->lblHelloMessage->setText(welcomeMessage);

	QString nickNameTemplate = R"(
            <p style="font-size: 10px; text-align: center;">
                <span style="color: black;">%1</span>
            </p>
        )";
	GBSLiveAccountInfo account = GBSMainCollector::getInstance()->getAccountInfo();

	QString nickName = nickNameTemplate.arg(QString::fromStdString(account.getNickname()));
	ui->lblNickName->setText(nickName);

	VertNaviButton *btnBasic = new VertNaviButton("  基础设置", ":gbs/images/gbs/biz/gbs-setting-basic.png", this);
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

	vertNaviButtons << btnBasic << btnProductDupRM << btnTimbreSquare << btnBridgerMgr << btnLiveEditor
			<< btnLiveSourcDupRM;

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


	connect(btnBasic, &QPushButton::clicked, this, &GBSNaviSetting::onBaiscClick);
	connect(btnProductDupRM, &QPushButton::clicked, this, &GBSNaviSetting::onProductDupRMClick);
	connect(btnTimbreSquare, &QPushButton::clicked, this, &GBSNaviSetting::onTimbreSquareClick);
	connect(btnBridgerMgr, &QPushButton::clicked, this, &GBSNaviSetting::onBridgerMgrClick);
	connect(btnLiveEditor, &QPushButton::clicked, this, &GBSNaviSetting::onLiveEditorClick);
	connect(btnLiveSourcDupRM, &QPushButton::clicked, this, &GBSNaviSetting::onLiveSourcDupRMClick);
	btnBasic->changeStyle(true);

	OBSBasic *main = OBSBasic::Get();
	QString path = main->getAvator();
	if (!path.isEmpty()) {
		QPixmap pixmap(path);
		ui->imgAvator->setPixmap(pixmap.scaled(48, 48, Qt::KeepAspectRatio));
	}


	
	btnBridgerMgr->setVisible(false);
	btnLiveEditor->setVisible(false);

}

GBSNaviSetting::~GBSNaviSetting()
{
	delete ui;
}

void GBSNaviSetting::mariVertButton(VertNaviButton *button)
{
	for (int i = 0; i < vertNaviButtons.count(); i++) {
		VertNaviButton *it = vertNaviButtons.at(i);
		if (it == button) {
			it->changeStyle(true);
		} else {
			it->changeStyle(false);
		}
	}
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

		VertNaviButton *button = qobject_cast<VertNaviButton *>(sender());
		mariVertButton(button);
		//这里是否可以使用replaceWidget ???
		layout->removeWidget(currentWidgetRef);
		delete currentWidgetRef;
		GBSBizSettingPage *gbsBizSettingPage = new GBSBizSettingPage(this);
		layout->addWidget(gbsBizSettingPage);
		currentWidgetRef = gbsBizSettingPage;
	}
}

void GBSNaviSetting::onProductDupRMClick()
{
	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {
		VertNaviButton *button = qobject_cast<VertNaviButton *>(sender());
		mariVertButton(button);
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
		VertNaviButton *button = qobject_cast<VertNaviButton *>(sender());
		mariVertButton(button);
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
		VertNaviButton *button = qobject_cast<VertNaviButton *>(sender());
		mariVertButton(button);
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
		VertNaviButton *button = qobject_cast<VertNaviButton *>(sender());
		mariVertButton(button);
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
		VertNaviButton *button = qobject_cast<VertNaviButton *>(sender());
		mariVertButton(button);
		layout->removeWidget(currentWidgetRef);
		delete currentWidgetRef;

		GBSBizSettingLiveSourceDupRM *gbsBizSettingLiveSourceDupRM = new GBSBizSettingLiveSourceDupRM(this);
		layout->addWidget(gbsBizSettingLiveSourceDupRM);
		currentWidgetRef = gbsBizSettingLiveSourceDupRM;
	}
}
//
//void GBSNaviSetting::onOutputClick()
//{
//	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
//	if (layout) {
//
//		VertNaviButton *button = qobject_cast<VertNaviButton *>(sender());
//		mariVertButton(button);
//		layout->removeWidget(currentWidgetRef);
//		delete currentWidgetRef;
//
//		GBSBizSettingOutput *gbsBizSettingOutput = new GBSBizSettingOutput(this);
//		layout->addWidget(gbsBizSettingOutput);
//		currentWidgetRef = gbsBizSettingOutput;
//
//	}
//}
//void GBSNaviSetting::onAVClick() {
//	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
//	if (layout) {
//		VertNaviButton *button = qobject_cast<VertNaviButton *>(sender());
//		mariVertButton(button);
//		layout->removeWidget(currentWidgetRef);
//		delete currentWidgetRef;
//		GBSBizSettingAV* gbsBizSettingAV = new GBSBizSettingAV(this);
//		layout->addWidget(gbsBizSettingAV);
//		currentWidgetRef = gbsBizSettingAV;
//
//
//
//	}
//}
