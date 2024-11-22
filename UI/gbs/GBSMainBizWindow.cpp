#include "gbsmainbizwindow.h"
#include "ui_gbsmainbizwindow.h"
#include <QPushButton>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include "naviWidgets/gbsnavidata.h"
#include "naviWidgets/gbsnaviproduct.h"
#include "naviWidgets/gbsnavilive.h"
#include "naviWidgets/gbsnavitranslate.h"
#include "naviWidgets/gbsnavisetting.h"
#include "naviWidgets/gbsnaviai.h"


#include "bizWidgets/GBSBizDeviceInfo.h"
#include "bizWidgets/GBSBizEShopData.h"
#include "bizWidgets/GBSBizAIData.h"
#include "bizWidgets/GBSBizSoYoung.h"


#include "bizWidgets/GBSBizSettingPage.h"
#include "bizWidgets/GBSBizSettingOutput.h"
#include "bizWidgets/GBSBizSettingAV.h"


#include "bizWidgets/GBSBizLivePusher.h"
#include "bizWidgets/GBSBizLiveBroker.h"

#include "gbs/common/GBSHttpClient.h"
#include "gbs/common/HoriNaviButton.h"
#include "gbs/common/QBizLogger.h"
#include "gbs/common/QToast.h"


#include "GBSMainProfile.h"
#include <thread>




GBSMainBizWindow::GBSMainBizWindow(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GBSMainBizWindow)
{
	ui->setupUi(this);
	currentHoriButtonIndex = 2;
	currentVertButtonIndex = 1;
	setSizePolicy(
		QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint |
		       Qt::WindowMaximizeButtonHint |
		       Qt::WindowCloseButtonHint);
	QLogD("Enter MainBiz");
	ui->imgLogo->setStyleSheet("border-image:url(:gbs/images/gbs/biz/gbs-logo.png)");
	ui->imgBanner->setStyleSheet("border-image:url(:gbs/images/gbs/biz/gbs-gbcg-banner.png)");
	
	setWindowTitle(OBSBasic::GetVendor() + " 直播软件");


	// ui->btnData->setStyleSheet(
	// 	"QPushButton {"
	// 	"   background-color: #9CA4AB;"
	// 	"   border: none;" // 无边框
	// 	"   border-radius: 5px;" // 圆角
	// 	"   font-size: 14px;"
	// 	"   text-align: center;"
	// 	"}"
	// 	"QPushButton:hover {"
	// 	"   background-color: #EB3F5E;"
	// 	"}"
	// 	"QPushButton:pressed {"
	// 	"   background-color: #F9F9F9;" // 按下状态背景颜色
	// 	"   padding-left: 1px;  /* 向左移动 3px */"
	// 	"   padding-top: 1px;    /* 向上移动 3px */"
	// 	"}"
	// 	);



	ui->btnNotification->setStyleSheet(
		"QPushButton {"
		"   background-image: url(:gbs/images/gbs/biz/gbs-receive-notificaton.png);"
		"   background-repeat: no-repeat;"
		"   background-position: center;"
		"   color: white;"
		"   border: none;" // 无边框
		"   border-radius: 5px;" // 圆角
		"   font-size: 16px;"
		"   padding: 0;" // 不添加内边距
		"}"
		"QPushButton:hover {"
		"   background-color: #EB3F5E;"
		"}"

		"QPushButton:pressed {"
		"   padding-left: 1px;  /* 向左移动 3px */"
		"   padding-top: 1px;    /* 向上移动 3px */"
		"}"
		);
	ui->btnMinimize->setStyleSheet(
		"QPushButton {"
		"   background-image: url(:gbs/images/gbs/biz/gbs-minimize-window.png);"
		"   background-repeat: no-repeat;"
		"   background-position: center;"
		"   color: white;"
		"   border: none;" // 无边框
		"   border-radius: 5px;" // 圆角
		"   font-size: 16px;"
		"   padding: 0;" // 不添加内边距
		"}"
		"QPushButton:hover {"
		"   background-color: #EB3F5E;"
		"}"
		"QPushButton:pressed {"
		"   padding-left: 1px;  /* 向左移动 3px */"
		"   padding-top: 1px;    /* 向上移动 3px */"
		"}"
		);
	ui->btnClose->setStyleSheet(
		"QPushButton {"
		"   background-image: url(:gbs/images/gbs/biz/gbs-close-window.png);"
		"   background-repeat: no-repeat;"
		"   background-position: center;"
		"   color: white;"
		"   border: none;" // 无边框
		"   border-radius: 5px;" // 圆角
		"   font-size: 16px;"
		"   padding: 0;" // 不添加内边距
		"}"
		"QPushButton:hover {"
		"   background-color: #EB3F5E;"
		"}"
		"QPushButton:pressed {"
		"   padding-left: 1px;  /* 向左移动 3px */"
		"   padding-top: 1px;    /* 向上移动 3px */"
		"}"
		);
	ui->btnProfile->setStyleSheet(
		"QPushButton {"
		"   background-image: url(:gbs/images/gbs/biz/gbs-my-profile.png);"
		"   background-repeat: no-repeat;"
		"   background-position: center;"
		"   color: white;"
		"   border: none;" // 无边框
		"   border-radius: 5px;" // 圆角
		"   font-size: 16px;"
		"   padding: 0;" // 不添加内边距
		"}"
		"QPushButton:hover {"
		"   background-color: #EB3F5E;"
		"}"
		"QPushButton:pressed {"
		"   padding-left: 1px;  /* 向左移动 3px */"
		"   padding-top: 1px;    /* 向上移动 3px */"
		"}"
		);



	horiButtons.append(ui->btnData);
	horiButtons.append(ui->btnProduct);
	horiButtons.append(ui->btnLive);
	horiButtons.append(ui->btnTranslate);
	horiButtons.append(ui->btnSetting);
	horiButtons.append(ui->btnAI);

	ui->btnData->changeStyle(true);

	GBSNaviData *gbsNaviData = new GBSNaviData(this);
	QHBoxLayout* bizPageLayout = new QHBoxLayout;
	naviLayout = new QHBoxLayout;
	bizLayout.reset(new QHBoxLayout);
	GBSBizDeviceInfo *gbsBizDevieInfo = new GBSBizDeviceInfo(this);
	bizPageLayout->addLayout(naviLayout);
	bizPageLayout->addLayout(bizLayout.data());
	bizPageLayout->setStretch(0, 205);
	bizPageLayout->setStretch(1, 1234);
	naviLayout->addWidget(gbsNaviData);
	bizLayout->addWidget(gbsBizDevieInfo);
	currentBizWidget = gbsBizDevieInfo;

	connect(ui->btnClose, &QPushButton::clicked, this, [this]() {
		qDebug() << "FILE: " << __FILE__ << " line " << __LINE__;

		OBSBasic *mainWindow = OBSBasic::Get();
		if (mainWindow) {
			qDebug() << "FILE: " << __FILE__ << " line " << __LINE__;

			//mainWindow->close(); // 将触发 closeEvent
		}
		qDebug() << "FILE: " << __FILE__ << " line " << __LINE__;

		QCoreApplication::exit(0);
		qDebug() << "FILE: " << __FILE__ << " line " << __LINE__;
	});

	//connect(ui->btnClose, &QPushButton::clicked, this, &GBSMainBizWindow::close);

	connect(ui->btnMinimize, &QPushButton::clicked, this, &GBSMainBizWindow::showMinimized);
	ui->verticalLayout->setSpacing(0);
	ui->verticalLayout->addWidget(ui->horizontalNavi);
	ui->verticalLayout->addLayout(bizPageLayout);
	ui->verticalLayout->setStretch(0, 72);
	ui->verticalLayout->setStretch(1, 918);


	//横向导航菜单按钮处理事件
	connect(ui->btnData, &QPushButton::clicked, this, &GBSMainBizWindow::onDataClick);
	connect(ui->btnProduct, &QPushButton::clicked, this, &GBSMainBizWindow::onProductClick);
	connect(ui->btnLive, &QPushButton::clicked, this, &GBSMainBizWindow::onLiveClick);
	connect(ui->btnTranslate, &QPushButton::clicked, this, &GBSMainBizWindow::onTranslateClick);
	connect(ui->btnSetting, &QPushButton::clicked, this, &GBSMainBizWindow::onSettingClick);
	connect(ui->btnAI, &QPushButton::clicked, this, &GBSMainBizWindow::onAIClick);
	GBSHttpClient::getInstance()->registerHandler(this);


	GBSHttpClient::getInstance()->queryByEquipmentNo();
	GBSHttpClient::getInstance()->getUserInfo();
	
	//std::this_thread::sleep_for(std::chrono::microseconds(2000));
	//connect(ui->btnClose, &QPushButton::clicked, this,
	//	&GBSMainBizWindow::closeWindow);



	connect(ui->btnProfile, &QPushButton::clicked, this, &GBSMainBizWindow::onPopupProfile);

	
    ui->btnData->setRealText("数据信息");
    ui->btnProduct->setRealText("全球货源");
    ui->btnLive->setRealText("远程代播");
    ui->btnTranslate->setRealText("实时翻译");
    ui->btnSetting->setRealText("直播设置");
    ui->btnAI->setRealText("大模型");
}

void GBSMainBizWindow::closeWindow()
{
	emit windowClosed();
}

void GBSMainBizWindow::onPopupProfile(bool checked) {
	GBSMainProfile *profile = new GBSMainProfile();
	profile->setWindowFlags(Qt::Popup);

	QPoint buttonGlobalPos = ui->btnProfile->mapToGlobal(QPoint(ui->btnProfile->width(), ui->btnProfile->height()));
	profile->move(buttonGlobalPos.x() - profile->width(), buttonGlobalPos.y());

	profile->show();
}

void GBSMainBizWindow::clearWidgetsFromLayout(QBoxLayout* layout) {
	QLayoutItem *child;
	while ((child = layout->takeAt(0)) != 0)
	{
		//setParent为NULL，防止删除之后界面不消失
		if(child->widget())
		{
			if (qobject_cast<GBSBizLiveBroker *>(child->widget())) {
				child->widget()->hide();
				update();
				return;
			}
			child->widget()->setParent(NULL);
			delete child->widget();//释放
		}

		delete child;
	}

}
void GBSMainBizWindow::markHoriButton(HoriNaviButton* button) {
    for(int i = 0; i < horiButtons.count(); i++) {
        HoriNaviButton* it = qobject_cast<HoriNaviButton*> (horiButtons.at(i));
        if (button ==  it) {
            it->changeStyle(true);
        } else {
            it->changeStyle(false);
        }
    }
}

void GBSMainBizWindow::onLoginResult(const int result) {}
void GBSMainBizWindow::onRtmpPushUrl(const std::string url) {}
void GBSMainBizWindow::onPullRtmpUrl(const std::string url) {}
void GBSMainBizWindow::onUserInfo(const GBSUserInfo *info) {
	int userId = info->GetId();
	
	if (!WebSocketClient::getInstance()->IsRunnig()) {
		QLogD("Start Weboscket userid %d", userId);
		WebSocketClient::getInstance()->Start(
			"wss://guobowss.guobo.shop/adminDistributeGoods/" +
			std::to_string(userId));
	}
}
void GBSMainBizWindow::onUserFileDownLoad(const std::string &path, int type) {}
void GBSMainBizWindow::onRoomInfos(std::list<GBSRoomInfo> &info) {}
void GBSMainBizWindow::onRoomInfo(GBSRoomInfo *info) {}
void GBSMainBizWindow::onQRcodeInfo(std::string no, std::string url, int status) {}

void GBSMainBizWindow::seeYouNext() {

	QToast *toast = new QToast(this, "感谢你的关注!\n我们已经在努力开发中，敬请期待未来的版本...", 3000);
	toast->show();
	return;
}


void GBSMainBizWindow::onDataClick(bool checked) {
    HoriNaviButton *button = qobject_cast<HoriNaviButton *>(sender());
    markHoriButton(button);
	clearWidgetsFromLayout(naviLayout);
	GBSNaviData *gbsNaviData = new GBSNaviData(this);
	naviLayout->addWidget(gbsNaviData);

	clearWidgetsFromLayout(bizLayout.data());
	GBSBizDeviceInfo* gbsDeviceInfo = new GBSBizDeviceInfo(this);
	bizLayout->addWidget(gbsDeviceInfo);
	gbsNaviData->addLayoutRef(bizLayout, gbsDeviceInfo);

	currentBizWidget = gbsDeviceInfo;


}
void GBSMainBizWindow::onProductClick(bool checked) {

	return seeYouNext();
	HoriNaviButton *button = qobject_cast<HoriNaviButton *>(sender());
	markHoriButton(button);
	clearWidgetsFromLayout(naviLayout);
	GBSNaviProduct *gbsNaviProduct = new GBSNaviProduct(this);
	naviLayout->addWidget(gbsNaviProduct);

	clearWidgetsFromLayout(bizLayout.data());
	GBSBizSoYoung *gbsSoYoung = new GBSBizSoYoung(this);
	bizLayout->addWidget(gbsSoYoung);
	gbsNaviProduct->addLayoutRef(bizLayout, gbsSoYoung);
	currentBizWidget = gbsSoYoung;
}
void GBSMainBizWindow::onLiveClick(bool checked) {
	HoriNaviButton *button = qobject_cast<HoriNaviButton *>(sender());
	markHoriButton(button);
	clearWidgetsFromLayout(naviLayout);
	GBSNaviLive* gbsNaviLive = new GBSNaviLive(this);
	naviLayout->addWidget(gbsNaviLive);
	clearWidgetsFromLayout(bizLayout.data());

	//GBSBizLivePusher* gbsBizlivePusher = new GBSBizLivePusher(this);
	//bizLayout->addWidget(gbsBizlivePusher);
	//gbsNaviLive->addLayoutRef(bizLayout, gbsBizlivePusher);
	//currentBizWidget = gbsBizlivePusher;

	GBSBizLiveBroker*  gbsBizLiveBroker= reinterpret_cast<GBSBizLiveBroker *>(App()->GetMainWindow());
	gbsNaviLive->addLayoutRef(bizLayout, gbsBizLiveBroker);
	bizLayout->addWidget(gbsBizLiveBroker);
	currentBizWidget = gbsBizLiveBroker;



}
void GBSMainBizWindow::onTranslateClick(bool checked) {
	return seeYouNext();
	HoriNaviButton *button = qobject_cast<HoriNaviButton *>(sender());
	markHoriButton(button);
	clearWidgetsFromLayout(naviLayout);
	GBSNaviTranslate *gbsNaviTranslate = new GBSNaviTranslate(this);
	naviLayout->addWidget(gbsNaviTranslate);
	clearWidgetsFromLayout(bizLayout.data());
	GBSBizSoYoung *gbsBizSoYoung = new GBSBizSoYoung(this);
	bizLayout->addWidget(gbsBizSoYoung);
	gbsNaviTranslate->addLayoutRef(bizLayout, gbsBizSoYoung);
	currentBizWidget = gbsBizSoYoung;
}
void GBSMainBizWindow::onSettingClick(bool checked) {
    HoriNaviButton *button = qobject_cast<HoriNaviButton *>(sender());
    markHoriButton(button);
	clearWidgetsFromLayout(naviLayout);
	GBSNaviSetting *gbsNaviSetting = new GBSNaviSetting(this);
	naviLayout->addWidget(gbsNaviSetting);
	clearWidgetsFromLayout(bizLayout.data());
	GBSBizSettingPage *gbsBizSettingPage = new GBSBizSettingPage(this);
	bizLayout->addWidget(gbsBizSettingPage);
	gbsNaviSetting->addLayoutRef(bizLayout, gbsBizSettingPage);
	currentBizWidget = gbsBizSettingPage;



}
void GBSMainBizWindow::onAIClick(bool checked) {
    HoriNaviButton *button = qobject_cast<HoriNaviButton *>(sender());
    markHoriButton(button);
	clearWidgetsFromLayout(naviLayout);
	GBSNaviAI *gbsNaviAI = new GBSNaviAI(this);
	naviLayout->addWidget(gbsNaviAI);
	clearWidgetsFromLayout(bizLayout.data());
	GBSBizSoYoung *gbsBizSoYoung = new GBSBizSoYoung(this);
	bizLayout->addWidget(gbsBizSoYoung);
	gbsNaviAI->addLayoutRef(bizLayout, gbsBizSoYoung);
}



GBSMainBizWindow::~GBSMainBizWindow()
{
	GBSHttpClient::getInstance()->unRegisterHandler(this);
	horiButtons.clear();
	vertButtons.clear();
	QLogD("Leave MainBiz");
	delete ui;
}

void GBSMainBizWindow::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down) {

		if (event->key() == Qt::Key_Up) {
			currentVertButtonIndex = (currentVertButtonIndex - 1 + vertButtons.size()) % vertButtons.size();
		} else if (event->key() == Qt::Key_Down) {
			currentVertButtonIndex = (currentVertButtonIndex + 1) % vertButtons.size();
		}

		//vertButtons[currentVertButtonIndex]->click();

		QPushButton *button =vertButtons[currentVertButtonIndex];
		// 模拟按钮按下和松开的过程
		QTimer::singleShot(1000, [button]() {
			// 模拟按钮按下
			Q_EMIT button->pressed();
			button->setDown(true);  // 视觉上表现为按下
			button->repaint();

			// 0.2秒后模拟松开按钮
			QTimer::singleShot(200, [button]() {
				Q_EMIT button->released();
				button->setDown(false);  // 恢复正常状态
				button->repaint();
				Q_EMIT button->clicked();  // 模拟点击信号
			});
		});
	}
	else if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right) {

		if (event->key() == Qt::Key_Left) {
			currentHoriButtonIndex = (currentHoriButtonIndex - 1 + horiButtons.size()) % horiButtons.size();
		} else if (event->key() == Qt::Key_Right) {
			currentHoriButtonIndex = (currentHoriButtonIndex + 1) % horiButtons.size();
		}

		//horiButtons[currentHoriButtonIndex]->click();

		QPushButton *button =horiButtons[currentHoriButtonIndex];

		// 模拟按钮按下和松开的过程
		QTimer::singleShot(2000, [button]() {
			// 模拟按钮按下
			Q_EMIT button->pressed();

			button->setDown(true);  // 视觉上表现为按下
			button->repaint();

			// 0.2秒后模拟松开按钮
			QTimer::singleShot(200, [button]() {
				Q_EMIT button->released();
				button->setDown(false);  // 恢复正常状态
				button->repaint();

				Q_EMIT button->clicked();  // 模拟点击信号
			});
		});

	}


}


void GBSMainBizWindow::mousePressEvent(QMouseEvent *event)
{
	// 记录鼠标按下的位置
	if (event->button() == Qt::LeftButton) {
		m_dragging = true;
		m_dragStartPos = event->globalPosition().toPoint() - pos();
	}
}

void GBSMainBizWindow::mouseMoveEvent(QMouseEvent *event)
{
	// 处理窗口拖动
	if (m_dragging) {
		move(event->globalPosition().toPoint() - m_dragStartPos);
	}
}

void GBSMainBizWindow::mouseReleaseEvent(QMouseEvent *event)
{
	// 释放鼠标，停止拖动
	if (event->button() == Qt::LeftButton) {
		m_dragging = false;
	}
}
// void GBSMainBizWindow::wheelEvent(QWheelEvent *event)
// {
// 	if (event->modifiers() & Qt::ControlModifier) {
// 		int delta = event->angleDelta().y();
// 		if (delta > 0) {
// 			resize(width() * 1.1, height() * 1.1);
// 		} else if (delta < 0) {
// 			resize(width() * 0.9, height() * 0.9);
// 		}
// 	}
// }
