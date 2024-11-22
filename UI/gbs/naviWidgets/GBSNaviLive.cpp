#include "gbsnavilive.h"
#include "ui_gbsnavilive.h"
#include "window-basic-main.hpp"
#include "../common/VertNaviButton.h"

#include "../bizWidgets/GBSBizLivePusher.h"
#include "../bizWidgets/GBSBizLiveBridger.h"
#include "../bizWidgets/GBSBizLiveBroker.h"
#include "../bizWidgets/GBSBizLiveDanmaku.h"
#include "../bizWidgets/GBSBizLiveGuarder.h"
#include "gbs/common/QBizLogger.h"


GBSNaviLive::GBSNaviLive(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GBSNaviLive)
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

	ui->lblNaviTitle->setText("远程代播");

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


	VertNaviButton* btnDBZB = new VertNaviButton("代播直播", ":gbs/images/gbs/biz/gbs-live-dbzb.png", this);
	VertNaviButton* btnCKGL = new VertNaviButton("场控管理", ":gbs/images/gbs/biz/gbs-live-ckgl.png", this);
	VertNaviButton* btnDMSZ = new VertNaviButton("弹幕设置", ":gbs/images/gbs/biz/gbs-live-dmsz.png", this);
	VertNaviButton* btnZBZB = new VertNaviButton("主播直播", ":gbs/images/gbs/biz/gbs-live-zbzb.png", this);

	vertNaviButtons << btnZBZB << btnDBZB << btnDMSZ << btnCKGL;
	btnDBZB->changeStyle(true);

	btnZBZB->setFixedSize(205, 40);
	btnDBZB->setFixedSize(205, 40);
	btnDMSZ->setFixedSize(205, 40);
	btnCKGL->setFixedSize(205, 40);
	ui->verticalLayout->addWidget(btnCKGL);
	ui->verticalLayout->addWidget(btnDBZB);
	ui->verticalLayout->addWidget(btnDMSZ);
	ui->verticalLayout->addWidget(btnZBZB);

	QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	ui->verticalLayout->addSpacerItem(verticalSpacer);

	connect(btnZBZB, &QPushButton::clicked, this,
		&GBSNaviLive::onZBZBClicked);
	connect(btnDBZB, &QPushButton::clicked, this,
		&GBSNaviLive::onDBZBClicked);
	connect(btnDMSZ, &QPushButton::clicked, this,
		&GBSNaviLive::onDMSZClicked);
	connect(btnCKGL, &QPushButton::clicked, this,
		&GBSNaviLive::onCKGLClicked);

	GBSHttpClient::getInstance()->pageQuery();
	
	WebSocketClient::getInstance()->RegisterHandler(this);
	
	gbsBizLiveBroker = reinterpret_cast<GBSBizLiveBroker *>(App()->GetMainWindow());
	OBSBasic *main = OBSBasic::Get();
	QString path = main->getRoundedAvator();
	QPixmap pixmap(path);
	ui->imgAvator->setPixmap(pixmap.scaled(48, 48, Qt::KeepAspectRatio));

	useLiveBroker = true;

}

void GBSNaviLive::addLayoutRef(QSharedPointer<QLayout> layout, QWidget* widget) {
	weakLayoutPtr = layout;
	currentWidgetRef = widget;
	qDebug() << "addLayoutRef debug layout ptr " << layout.data();
}


GBSNaviLive::~GBSNaviLive()
{
	WebSocketClient::getInstance()->UnRegisterHandler(this);
	
	delete ui;
}

void GBSNaviLive::mariVertButton(VertNaviButton *button)
{
	for (int i = 0; i < vertNaviButtons.count(); i++) {
		VertNaviButton *it = vertNaviButtons.at(i);
		if (it == button) {
			it->changeStyle(true);
		} else {
			it->changeStyle(false);
		}
	}
	update();
}

void GBSNaviLive::onZBZBClicked() {

	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {
		VertNaviButton *button = qobject_cast<VertNaviButton *>(sender());
		mariVertButton(button);

		//这里是否可以使用replaceWidget ???
		layout->removeWidget(currentWidgetRef);
		currentWidgetRef->hide();
		if (!useLiveBroker) {
			currentWidgetRef->deleteLater();
		} else {
			gbsBizLiveBroker->endPullTask();
		}
		currentWidgetRef = nullptr;
		GBSBizLivePusher* gbsBizLivePusher = new GBSBizLivePusher(this);
		layout->addWidget(gbsBizLivePusher);
		currentWidgetRef = gbsBizLivePusher;
		//connect(this, &GBSNaviLive::signalDanmakuReceived,
		//	gbsBizLivePusher, &GBSBizLivePusher::addNewWidget);

	}
	layout->invalidate(); // 使布局无效并重新调整
	useLiveBroker = false;


}
void GBSNaviLive::onDBZBClicked() {

	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout && !useLiveBroker) {
		VertNaviButton *button = qobject_cast<VertNaviButton *>(sender());
		mariVertButton(button);

		layout->removeWidget(currentWidgetRef);
		currentWidgetRef->hide();
		currentWidgetRef->deleteLater();
		//gbsBizLiveBroker->OBSInit();
		layout->addWidget(gbsBizLiveBroker);
		currentWidgetRef = gbsBizLiveBroker;
		gbsBizLiveBroker->show();
		gbsBizLiveBroker->beginPullTask();

	}
	layout->invalidate(); // 使布局无效并重新调整

	useLiveBroker = true;

}
void GBSNaviLive::onDMSZClicked() {
	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {
		VertNaviButton *button = qobject_cast<VertNaviButton *>(sender());
		mariVertButton(button);
		layout->removeWidget(currentWidgetRef);
		currentWidgetRef->hide();
		if (!useLiveBroker) {
			currentWidgetRef->deleteLater();
		} else {
			gbsBizLiveBroker->endPullTask();
		}
		currentWidgetRef = nullptr;
		GBSBizLiveDanmaku *gbsBizLiveDanmaku =
			new GBSBizLiveDanmaku(this);
		layout->addWidget(gbsBizLiveDanmaku);
		currentWidgetRef = gbsBizLiveDanmaku;
	}
	layout->invalidate(); // 使布局无效并重新调整

	useLiveBroker = false;

}
void GBSNaviLive::onCKGLClicked() {
	QSharedPointer<QLayout> layout = weakLayoutPtr.toStrongRef();
	if (layout) {
		VertNaviButton *button = qobject_cast<VertNaviButton *>(sender());
		mariVertButton(button);
		layout->removeWidget(currentWidgetRef);
		currentWidgetRef->hide();
		if (!useLiveBroker) {
			currentWidgetRef->deleteLater();
		} else {
			gbsBizLiveBroker->endPullTask();
		}
		currentWidgetRef = nullptr;
		GBSBizLiveGuarder *gbsBizLiveGuarder = new GBSBizLiveGuarder(this);
		layout->addWidget(gbsBizLiveGuarder);
		currentWidgetRef = gbsBizLiveGuarder;
	}
	layout->invalidate(); // 使布局无效并重新调整

	useLiveBroker = false;

}

void GBSNaviLive::onMessage(std::string msg) {
	try {
		auto jsonObject = nlohmann::json::parse(msg);
		if (jsonObject.is_object()) {
			processDanmaItem(jsonObject);
		} else if (jsonObject.is_array()) {
			int size = (int)jsonObject.size();
			for (int i = 0; i < size; i++) {
				processDanmaItem(jsonObject[i]);
			}
		}
	} catch (const nlohmann::json::parse_error &e) {
		qDebug() << "JSON 解析错误: " << msg
			 << " Execption: " << e.what();
	} catch (const std::exception &e) {
		qDebug() << "标准异常: " << msg << " Execption: " << e.what();
	}

}


void GBSNaviLive::onOpen() {}

void GBSNaviLive::onClose() {}


void GBSNaviLive::processDanmaItem(const nlohmann::json jsonObject) {
	QString plat = "other";
	if (danmaPlatIconString.isEmpty()) {
		std::unique_ptr<IniSettings> iniFile = std::make_unique<IniSettings>("danmu/setting/setting.ini");
		plat = iniFile->value("broadcast", "plat", "other").toString();
		if (plat == "douyin") {
			danmaPlatIconString = ":gbs/images/gbs/biz/gbs-logo-douyin.png";
		} else if (plat == "kuaishou") {
			danmaPlatIconString = ":gbs/images/gbs/biz/gbs-logo-kuai.png";
		} else if (plat == "shipinhao") {
			danmaPlatIconString = ":gbs/images/gbs/biz/gbs-logo-wechat.png";
		} else if (plat == "tiktok") {
			danmaPlatIconString = ":gbs/images/gbs/biz/gbs-logo-tiktok.png";
		} else if (plat == "bili") {
			danmaPlatIconString = ":gbs/images/gbs/biz/gbs-logo-bilibili.png";
		} else if (plat == "pdd") {
			danmaPlatIconString = ":gbs/images/gbs/biz/gbs-logo-bilibili.png";
		} else if (plat == "facebook") {
			danmaPlatIconString = ":gbs/images/gbs/biz/gbs-logo-facebook.png";
		} else if (plat == "other") {
			
		}
	}
	if (danmaPlatIconString.isEmpty()) {
		QLogE("No platform about danmaku, ProcessDanmaItem failed.");
		return;
	}

	std::string type = jsonObject["type"].get<std::string>();
	qDebug() << "damaku type " << type;
	if (type == "MemberMessage") {
		auto danma = std::make_shared<DammaMemberMSG>();
		danma->type = "MemberMessage";
		danma->name = jsonObject["name"].get<std::string>();
		danma->head_image = jsonObject["head_image"].get<std::string>();
		
		danma->content = jsonObject["content"].get<std::string>();
		danma->msgType = 1;
		 
		QString danmaText = QString::fromStdString(danma->name) + ":" +
				    QString::fromStdString(danma->content);
		emit signalDanmakuReceived("D01", danmaPlatIconString, danmaText, QString::fromStdString(type));

	} else if (type == "ChatMessage") {
		auto danma = std::make_shared<DanmaChatMessage>();
		danma->type = "ChatMessage";
		danma->name = jsonObject["name"].get<std::string>();
		danma->head_image = jsonObject["head_image"].get<std::string>();
		danma->content = jsonObject["content"].get<std::string>();
		danma->msgType = 2;
		QString danmaText = QString::fromStdString(danma->name) + ":" +
				    QString::fromStdString(danma->content);
		emit signalDanmakuReceived("D01", danmaPlatIconString, danmaText, QString::fromStdString(type));

	} else if (type == "GiftMessage") {
		auto danma = std::make_shared<DanmaGiftMessage>();
		danma->type = "GiftMessage";
		danma->name = jsonObject["name"].get<std::string>();
		danma->head_image = jsonObject["head_image"].get<std::string>();
		danma->content = jsonObject["content"].get<std::string>();
		danma->gift_name = jsonObject["gift_name"].get<std::string>();
		danma->gift_count = jsonObject["gift_count"].get<std::string>();
		danma->msgType = 3;
		QString danmaText = QString::fromStdString(danma->name) + ":" +
				    QString::fromStdString(danma->content);
		emit signalDanmakuReceived("D01", danmaPlatIconString, danmaText, QString::fromStdString(type));

	} else if (type == "SocialMessage") {
		auto danma = std::make_shared<DanmaSocialMessage>();
		danma->type = "SocialMessage";
		danma->name = jsonObject["name"].get<std::string>();
		danma->head_image = jsonObject["head_image"].get<std::string>();
		danma->content = jsonObject["content"].get<std::string>();
		danma->msgType = 4;
		QString danmaText = QString::fromStdString(danma->name) + ":" +
				    QString::fromStdString(danma->content);
		emit signalDanmakuReceived("D01", danmaPlatIconString, danmaText, QString::fromStdString(type));
	} else if (type == "LikeMessage") {
		auto danma = std::make_shared<DanmaLikeMessage>();
		danma->type = "LikeMessage";
		danma->name = jsonObject["name"].get<std::string>();
		danma->head_image = jsonObject["head_image"].get<std::string>();
		danma->content = jsonObject["content"].get<std::string>();
		danma->count = jsonObject["count"].get<std::string>();
		danma->msgType = 5;
		QString danmaText = QString::fromStdString(danma->name) + ":" +
				    QString::fromStdString(danma->content);
		emit signalDanmakuReceived("D01", danmaPlatIconString, danmaText, QString::fromStdString(type));

		
	}
}
