#include "gbsnaviproduct.h"
#include "ui_gbsnaviproduct.h"
#include "../common/VertNaviButton.h"
#include "../bizWidgets/GBSBizSoYoung.h"

GBSNaviProduct::GBSNaviProduct(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GBSNaviProduct)
{
	ui->setupUi(this);
	ui->imgWechatQrCode->setStyleSheet("border-image:url(:gbs/images/gbs/biz/gbs-wechat-qrcode.png)");

	ui->lblNaviTitle->setStyleSheet("QLabel {"
					"    background: transparent;"
					"    color: #1B2846;"     // 文本颜色
					"    font-size: 14px;"    // 字体大小
					"    padding-left: 15px;" // 左对齐并添加15px的内边距
					"}");

	ui->lblNaviTitle->setText("选品广场");

	VertNaviButton* btnBTTJ = new VertNaviButton("本土推荐", ":gbs/images/gbs/biz/gbs-product-bttj.png", this);
	VertNaviButton* btnXBWJ = new VertNaviButton("鞋包玩具", ":gbs/images/gbs/biz/gbs-product-xbwj.png", this);
	VertNaviButton* btnSMJD = new VertNaviButton("数码家电", ":gbs/images/gbs/biz/gbs-product-smjd.png", this);
	VertNaviButton* btnQCDM = new VertNaviButton("汽车电摩", ":gbs/images/gbs/biz/gbs-product-qcdm.png", this);
	VertNaviButton* btnJFSJ = new VertNaviButton("家纺丝巾", ":gbs/images/gbs/biz/gbs-product-jfsj.png", this);
	VertNaviButton* btnRYBH = new VertNaviButton("日用百货", ":gbs/images/gbs/biz/gbs-product-rybh.png", this);
	VertNaviButton* btnMZRH = new VertNaviButton("美妆日化", ":gbs/images/gbs/biz/gbs-product-mzrh.png", this);
	VertNaviButton* btnJXZM = new VertNaviButton("机械照明", ":gbs/images/gbs/biz/gbs-product-jxzm.png", this);
	VertNaviButton* btnBGWJ = new VertNaviButton("办公文具", ":gbs/images/gbs/biz/gbs-product-bgwj.png", this);

	btnBTTJ->setFixedSize(205, 40);
	btnXBWJ->setFixedSize(205, 40);
	btnSMJD->setFixedSize(205, 40);
	btnQCDM->setFixedSize(205, 40);
	btnJFSJ->setFixedSize(205, 40);
	btnRYBH->setFixedSize(205, 40);
	btnMZRH->setFixedSize(205, 40);
	btnJXZM->setFixedSize(205, 40);
	btnBGWJ->setFixedSize(205, 40);


	ui->verticalLayout->addWidget(btnBTTJ);
	ui->verticalLayout->addWidget(btnXBWJ);
	ui->verticalLayout->addWidget(btnSMJD);
	ui->verticalLayout->addWidget(btnQCDM);
	ui->verticalLayout->addWidget(btnJFSJ);
	ui->verticalLayout->addWidget(btnRYBH);
	ui->verticalLayout->addWidget(btnMZRH);
	ui->verticalLayout->addWidget(btnJXZM);
	ui->verticalLayout->addWidget(btnBGWJ);


	QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	ui->verticalLayout->addSpacerItem(verticalSpacer);

	connect(btnBTTJ, &VertNaviButton::clicked, this, &GBSNaviProduct::onBTTJClick);
	connect(btnXBWJ, &VertNaviButton::clicked, this, &GBSNaviProduct::onXBWJClick);
	connect(btnSMJD, &VertNaviButton::clicked, this, &GBSNaviProduct::onSMJDClick);
	connect(btnQCDM, &VertNaviButton::clicked, this, &GBSNaviProduct::onQCDMClick);
	connect(btnJFSJ, &VertNaviButton::clicked, this, &GBSNaviProduct::onJFSFClick);
	connect(btnRYBH, &VertNaviButton::clicked, this, &GBSNaviProduct::onRYBHClick);
	connect(btnMZRH, &VertNaviButton::clicked, this, &GBSNaviProduct::onMZRHClick);
	connect(btnJXZM, &VertNaviButton::clicked, this, &GBSNaviProduct::onJXZMClick);
	connect(btnBGWJ, &VertNaviButton::clicked, this, &GBSNaviProduct::onBGWJClick);
	vertNaviButtons << btnBTTJ << btnXBWJ << btnSMJD << btnQCDM << btnJFSJ << btnRYBH << btnMZRH << btnJXZM
			<< btnBGWJ;
	btnBTTJ->changeStyle(true);

	// ui->btnBGWJ->setStyleSheet(
	// 	"QPushButton {"
	// 	"   background-image: url(:gbs/images/gbs/biz/gbs-product-bgwj.png);"
	// 	"   background-repeat: no-repeat;"
	// 	"   background-position: center;"
	// 	"   color: white;"
	// 	"   border: none;" // 无边框
	// 	"   border-radius: 5px;" // 圆角
	// 	"   font-size: 16px;"
	// 	"   padding: 0;" // 不添加内边距
	// 	"}"
	// 	"QPushButton:hover {"
	// 	"   background-color: #1B2846;"
	// 	"}"
	// 	"QPushButton:pressed {"
	// 	"   padding-left: 1px;  /* 向左移动 3px */"
	// 	"   padding-top: 1px;    /* 向上移动 3px */"
	// 	"}"
	// 	);
}


GBSNaviProduct::~GBSNaviProduct()
{
	delete ui;
}

void GBSNaviProduct::mariVertButton(VertNaviButton *button)
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

void GBSNaviProduct::addLayoutRef(QSharedPointer<QLayout> layout, QWidget *widget)
{
	weakLayoutPtr = layout;
	currentWidgetRef = widget;
}
void GBSNaviProduct::onBTTJClick()
{
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

void GBSNaviProduct::onXBWJClick()
{
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

void GBSNaviProduct::onSMJDClick()
{
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

void GBSNaviProduct::onQCDMClick()
{
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

void GBSNaviProduct::onJFSFClick()
{
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

void GBSNaviProduct::onRYBHClick()
{
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

void GBSNaviProduct::onMZRHClick()
{
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
void GBSNaviProduct::onJXZMClick()
{
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
void GBSNaviProduct::onBGWJClick()
{
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


