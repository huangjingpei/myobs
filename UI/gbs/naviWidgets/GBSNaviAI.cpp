#include "gbsnaviai.h"
#include "ui_gbsnaviai.h"
#include "../common/VertNaviButton.h"
#include "../bizWidgets/GBSBizSoYoung.h"
#include "window-basic-main.hpp"

GBSNaviAI::GBSNaviAI(QWidget *parent) : QWidget(parent), ui(new Ui::GBSNaviAI)
{
	ui->setupUi(this);
	ui->imgTheme->setStyleSheet("border-image:url(:gbs/images/gbs/biz/gbs-theme-dark.png)");
	ui->imgTheme->setVisible(false);
	ui->radioButton->setVisible(false);
	ui->lblNaviTitle->setStyleSheet(
	"QLabel {"
	"    background: transparent;"
	"    color: #1B2846;"                        // 文本颜色
	"    font-size: 14px;"                     // 字体大小
	"    padding-left: 15px;"  // 左对齐并添加15px的内边距
	"}"
	);

	ui->lblNaviTitle->setText("大模型设置");



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
	VertNaviButton* btnFacefusion = new VertNaviButton("Facefusion (换脸)", ":gbs/images/gbs/biz/gbs-ai-facefusion.png", this);
	VertNaviButton* btnRVC = new VertNaviButton("RVC (实时变声)", ":gbs/images/gbs/biz/gbs-ai-rvc.png", this);
	VertNaviButton* btnLlama3 = new VertNaviButton("Llama3 (话术拆分)", ":gbs/images/gbs/biz/gbs-ai-llama3.png", this);
	VertNaviButton* btnMuseTalk = new VertNaviButton("MuseTalk (数字人)", ":gbs/images/gbs/biz/gbs-ai-musetalk.png", this);

    vertNaviButtons << btnTTS << btnFacefusion << btnRVC << btnLlama3 << btnMuseTalk;

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
    btnTTS->changeStyle(true);


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

	OBSBasic *main = OBSBasic::Get();
    QString path = main->getRoundedAvator();
	QPixmap pixmap(path);
	ui->imgAvator->setPixmap(pixmap.scaled(48, 48, Qt::KeepAspectRatio));

}

GBSNaviAI::~GBSNaviAI()
{
	delete ui;
}

void GBSNaviAI::mariVertButton(VertNaviButton* button) {
    for(int i = 0; i < vertNaviButtons.count(); i++) {
        VertNaviButton * it = vertNaviButtons.at(i);
        if (it == button) {
            it->changeStyle(true);
        } else {
            it->changeStyle(false);
        }
    }
}
void GBSNaviAI::addLayoutRef(QSharedPointer<QLayout> layout, QWidget* widget) {
	weakLayoutPtr = layout;
	currentWidgetRef = widget;
	qDebug() << "addLayoutRef debug layout ptr " << layout.data();
}

void GBSNaviAI::onTTSlicked() {
    VertNaviButton *button = qobject_cast<VertNaviButton *>(sender());
    mariVertButton(button);
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
    VertNaviButton *button = qobject_cast<VertNaviButton *>(sender());
    mariVertButton(button);
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
    VertNaviButton *button = qobject_cast<VertNaviButton *>(sender());
    mariVertButton(button);
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
    VertNaviButton *button = qobject_cast<VertNaviButton *>(sender());
    mariVertButton(button);
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
    VertNaviButton *button = qobject_cast<VertNaviButton *>(sender());
    mariVertButton(button);
	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {

		layout->removeWidget(currentWidgetRef);
		delete currentWidgetRef;
		GBSBizSoYoung* gbsBizSoYoung = new GBSBizSoYoung(this);
		layout->addWidget(gbsBizSoYoung);
		currentWidgetRef = gbsBizSoYoung;
	}
}




