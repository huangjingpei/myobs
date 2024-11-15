#include "gbsnavitranslate.h"
#include "ui_gbsnavitranslate.h"

#include "../common/VertNaviButton.h"
#include "../bizWidgets/GBSBizSoYoung.h"
#include "window-basic-main.hpp"
GBSNaviTranslate::GBSNaviTranslate(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GBSNaviTranslate)
{
	ui->setupUi(this);

	ui->imgTheme->setStyleSheet("border-image:url(:gbs/images/gbs/biz/gbs-theme-dark.png)");


	ui->lblNaviTitle->setStyleSheet("QLabel {"
					"    background: transparent;"
					"    color: #1B2846;"     // 文本颜色
					"    font-size: 14px;"    // 字体大小
					"    padding-left: 15px;" // 左对齐并添加15px的内边距
					"}");

	ui->lblNaviTitle->setText("实时翻译");


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

	VertNaviButton* btnDMXHCYY = new VertNaviButton("大模型合成语音", ":gbs/images/gbs/biz/gbs-translate-intertrans.png", this);
	VertNaviButton* btnWBHCZB = new VertNaviButton("文本合成直播", ":gbs/images/gbs/biz/gbs-translate-speech.png", this);
	VertNaviButton* btnSPFYHC = new VertNaviButton("视频翻译合成", ":gbs/images/gbs/biz/gbs-translate-video.png", this);
	vertNaviButtons << btnDMXHCYY << btnWBHCZB << btnSPFYHC;
	btnDMXHCYY->changeStyle(true);
	btnDMXHCYY->setFixedSize(205, 40);
	btnWBHCZB->setFixedSize(205, 40);
	btnSPFYHC->setFixedSize(205, 40);
	ui->verticalLayout->addWidget(btnDMXHCYY);
	ui->verticalLayout->addWidget(btnWBHCZB);
	ui->verticalLayout->addWidget(btnSPFYHC);

	QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	ui->verticalLayout->addSpacerItem(verticalSpacer);



	// ui->btnDMXHCYY->setStyleSheet(
	// 	"QPushButton {"
	// 	"   background-image: url(:gbs/images/gbs/biz/gbs-translate-dmxhcyy.png);"
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


	connect(btnDMXHCYY, &QPushButton::clicked, this, &GBSNaviTranslate::onBigAIClick);
	connect(btnWBHCZB, &QPushButton::clicked, this, &GBSNaviTranslate::onTextAIClick);
	connect(btnSPFYHC, &QPushButton::clicked, this, &GBSNaviTranslate::onVideoAIClick);
	OBSBasic *main = OBSBasic::Get();
	QString path = main->getRoundedAvator();
	QPixmap pixmap(path);
	ui->imgAvator->setPixmap(pixmap.scaled(48, 48, Qt::KeepAspectRatio));

}

GBSNaviTranslate::~GBSNaviTranslate()
{
	delete ui;
}

void GBSNaviTranslate::mariVertButton(VertNaviButton *button)
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
void GBSNaviTranslate::addLayoutRef(QSharedPointer<QLayout> layout, QWidget* widget) {
	weakLayoutPtr = layout;
	currentWidgetRef = widget;
}


void GBSNaviTranslate::onBigAIClick() {

	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {
		VertNaviButton *button = qobject_cast<VertNaviButton *>(sender());
		mariVertButton(button);
		//这里是否可以使用replaceWidget ???
		layout->removeWidget(currentWidgetRef);
		delete currentWidgetRef;
		GBSBizSoYoung* gbsBizSoYoung = new GBSBizSoYoung(this);
		layout->addWidget(gbsBizSoYoung);
		currentWidgetRef = gbsBizSoYoung;
	}
}
void GBSNaviTranslate::onTextAIClick()
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
void GBSNaviTranslate::onVideoAIClick() {
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

