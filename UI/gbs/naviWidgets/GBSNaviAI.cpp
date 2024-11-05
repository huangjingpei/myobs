#include "gbsnaviai.h"
#include "ui_gbsnaviai.h"
#include "../common/VertNaviButton.h"
#include "../bizWidgets/GBSBizSoYoung.h"

GBSNaviAI::GBSNaviAI(QWidget *parent) : QWidget(parent), ui(new Ui::GBSNaviAI)
{
	ui->setupUi(this);
	ui->imgTheme->setStyleSheet("border-image:url(:gbs/images/gbs/biz/gbs-theme-dark.png)");
	ui->imgAvator->setStyleSheet("border-image:url(:gbs/images/gbs/biz/gbs-logo.png)");
	QString naviTitle = R"(
	    <p style="font-size: 16px; text-align: center;">
		<span style="color: #9CA4AB;">大模型</span>
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

	VertNaviButton* btnTTS = new VertNaviButton("TTS (语音合成)", ":gbs/images/gbs/biz/gbs-ai-tts.png", this);
	VertNaviButton* btnFacefusion = new VertNaviButton("Facefusion (换脸)", ":gbs/images/gbs/biz/gbs-ai-facefusion-dark.png", this);
	VertNaviButton* btnRVC = new VertNaviButton("RVC (实时变声)", ":gbs/images/gbs/biz/gbs-ai-rvc-dark.png", this);
	VertNaviButton* btnLlama3 = new VertNaviButton("Llama3 (话术拆分)", ":gbs/images/gbs/biz/gbs-ai-llama3-dark.png", this);
	VertNaviButton* btnMuseTalk = new VertNaviButton("MuseTalk (数字人)", ":gbs/images/gbs/biz/gbs-ai-musetalk-dark.png", this);


	btnTTS->setFixedSize(205, 40);
	btnFacefusion->setFixedSize(205, 40);
	btnRVC->setFixedSize(205, 40);
	btnLlama3->setFixedSize(205, 40);
	btnMuseTalk->setFixedSize(205, 40);

	ui->verticalLayout->addWidget(btnTTS);
	ui->verticalLayout->addWidget(btnFacefusion);
	ui->verticalLayout->addWidget(btnRVC);
	ui->verticalLayout->addWidget(btnLlama3);
	ui->verticalLayout->addWidget(btnMuseTalk);
	QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	ui->verticalLayout->addSpacerItem(verticalSpacer);

	connect(btnTTS, &QPushButton::clicked, this, &GBSNaviAI::onTTSlicked);
	connect(btnFacefusion, &QPushButton::clicked, this,
		&GBSNaviAI::onFacefusionlicked);
	connect(btnRVC, &QPushButton::clicked, this, &GBSNaviAI::onRVCClicked);
	connect(btnLlama3, &QPushButton::clicked, this,
		&GBSNaviAI::onLlama3Clicked);
	connect(btnMuseTalk, &QPushButton::clicked, this,
		&GBSNaviAI::onMuseTalkClicked);


	// ui->btnYYHC->setStyleSheet(
	// 	"QPushButton {"
	// 	"   background-image: url(:gbs/images/gbs/biz/gbs-ai-yyhc.png);"
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

GBSNaviAI::~GBSNaviAI()
{
	delete ui;
}

void GBSNaviAI::addLayoutRef(QSharedPointer<QLayout> layout, QWidget* widget) {
	weakLayoutPtr = layout;
	currentWidgetRef = widget;
	qDebug() << "addLayoutRef debug layout ptr " << layout.data();
}

void GBSNaviAI::onTTSlicked() {
	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {

		layout->removeWidget(currentWidgetRef);
		delete currentWidgetRef;
		GBSBizSoYoung* gbsBizSoYoung = new GBSBizSoYoung(this);
		layout->addWidget(gbsBizSoYoung);
		currentWidgetRef = gbsBizSoYoung;



	}
}

void GBSNaviAI::onFacefusionlicked() {

	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {
		//这里是否可以使用replaceWidget ???
		layout->removeWidget(currentWidgetRef);
		delete currentWidgetRef;
		GBSBizSoYoung* gbsBizSoYoung = new GBSBizSoYoung(this);
		layout->addWidget(gbsBizSoYoung);
		currentWidgetRef = gbsBizSoYoung;

	}
}

void GBSNaviAI::onRVCClicked()
{
	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {


		layout->removeWidget(currentWidgetRef);
		delete currentWidgetRef;
		GBSBizSoYoung* gbsBizSoYoung = new GBSBizSoYoung(this);
		layout->addWidget(gbsBizSoYoung);
		currentWidgetRef = gbsBizSoYoung;

	}
}

void GBSNaviAI::onLlama3Clicked() {
	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {

		layout->removeWidget(currentWidgetRef);
		delete currentWidgetRef;
		GBSBizSoYoung* gbsBizSoYoung = new GBSBizSoYoung(this);
		layout->addWidget(gbsBizSoYoung);
		currentWidgetRef = gbsBizSoYoung;




	}
}

void GBSNaviAI::onMuseTalkClicked() {
	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {

		layout->removeWidget(currentWidgetRef);
		delete currentWidgetRef;
		GBSBizSoYoung* gbsBizSoYoung = new GBSBizSoYoung(this);
		layout->addWidget(gbsBizSoYoung);
		currentWidgetRef = gbsBizSoYoung;
	}
}




