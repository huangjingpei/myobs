#include "gbsnavidata.h"
#include "ui_gbsnavidata.h"
#include "../common/VertNaviButton.h"
#include "../bizWidgets/GBSBizDeviceInfo.h"
#include "../bizWidgets/GBSBizSoYoung.h"
#include "window-basic-main.hpp"
#include "gbs/common/QToast.h"
#include "gbs/bizWidgets/GBSMsgDialog.h"
#include "gbs/common/QBizLogger.h"

GBSNaviData::GBSNaviData(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GBSNaviData)
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

	ui->lblNaviTitle->setText("数据大屏");


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
	vertNaviButtons << btnDeviceInfo << btnEShopData << btnAIData;
	btnDeviceInfo->changeStyle(true);

	btnDeviceInfo->setFixedSize(205, 40);
	btnEShopData->setFixedSize(205, 40);
	btnAIData->setFixedSize(205, 40);
	ui->verticalLayout->addWidget(btnDeviceInfo);
	ui->verticalLayout->addWidget(btnEShopData);
	ui->verticalLayout->addWidget(btnAIData);
	QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	ui->verticalLayout->addSpacerItem(verticalSpacer);

	connect(btnDeviceInfo, &VertNaviButton::clicked, this, &GBSNaviData::onDeviceInfoClicked);
	connect(btnEShopData, &VertNaviButton::clicked, this,
		&GBSNaviData::onEShopDataClicked);
	connect(btnAIData, &VertNaviButton::clicked, this,
		&GBSNaviData::onAIDataClicked);
	OBSBasic *main = OBSBasic::Get();

	connect(main, &OBSBasic::onUseIconUpdate, this, &GBSNaviData::onMyIconDownloaded);




}

GBSNaviData::~GBSNaviData()
{
	delete ui;
}

void GBSNaviData::seeYouNext(QString title)
{
	

	// QToast *toast = new QToast(mainWidget, "感谢你的关注!\n我们已经在努力开发中，敬请期待未来的版本...", 2000);
	// toast->show();
	// return;
	QWidget *widget = new QWidget;
	QVBoxLayout *layout = new QVBoxLayout(widget);
	QLabel *label = new QLabel("感谢你的关注 !\n我们已经在努力开发中，敬请期待未来的版本... ");
	QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Minimum);
	layout->addItem(spacer);
	label->setStyleSheet("font-size: 32px;");
	label->setAlignment(Qt::AlignHCenter);
	
	layout->addWidget(label);
	GBSMsgDialog *dialog = new GBSMsgDialog(title, layout, this);
	dialog->exec();
}

void GBSNaviData::setMainBizWindow(QWidget* widget) {
	mainWidget = widget;
}

void GBSNaviData::mariVertButton(VertNaviButton *button)
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

void GBSNaviData::addLayoutRef(QSharedPointer<QLayout> layout, QWidget* widget) {
	weakLayoutPtr = layout;
	currentWidgetRef = widget;
	qDebug() << "addLayoutRef debug layout ptr " << layout.data();
}

void GBSNaviData::onDeviceInfoClicked() {
	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {
		VertNaviButton *button = qobject_cast<VertNaviButton *>(sender());
		mariVertButton(button);
		layout->removeWidget(currentWidgetRef);
		delete currentWidgetRef;
		GBSBizDeviceInfo* gbsBizDeviceInfo = new GBSBizDeviceInfo(this);
		layout->addWidget(gbsBizDeviceInfo);
		currentWidgetRef = gbsBizDeviceInfo;



	}
}

void GBSNaviData::onEShopDataClicked() {
	return seeYouNext("电商数据");
	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {
		VertNaviButton *button = qobject_cast<VertNaviButton *>(sender());
		mariVertButton(button);
		layout->removeWidget(currentWidgetRef);
		delete currentWidgetRef;
		GBSBizSoYoung* gbsBizSoYoung = new GBSBizSoYoung(this);
		layout->addWidget(gbsBizSoYoung);
		currentWidgetRef = gbsBizSoYoung;
	}
}


void GBSNaviData::onAIDataClicked() {
	return seeYouNext("智能数据");
	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {
		VertNaviButton *button = qobject_cast<VertNaviButton *>(sender());
		mariVertButton(button);
		layout->removeWidget(currentWidgetRef);
		delete currentWidgetRef;
		GBSBizSoYoung* gbsBizSoYoung = new GBSBizSoYoung(this);
		layout->addWidget(gbsBizSoYoung);
		currentWidgetRef = gbsBizSoYoung;



	}
}


void GBSNaviData::onMyIconDownloaded(QString path) {

	QLogD("onMyIconDownloaded . %s.", path.toStdString().c_str());
	QPixmap pixmap(path);
	ui->imgAvator->setPixmap(pixmap.scaled(48, 48, Qt::KeepAspectRatio));
	ui->imgAvator->setStyleSheet(
		" border-radius: 50%;");
}
