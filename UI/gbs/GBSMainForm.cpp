#include "GBSMainForm.h"
#include "ui_GBSMainForm.h"
#include "gbsbasicwindow.h"
#include "gbsmainwindow.h"
#include "window-basic-main.hpp"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include "GBSNormalLoginForm.h"

#include "PixmapOverlay.h"
#include <QPixmap>

#include "GBSHttpsServer.h"


GBSMainForm::GBSMainForm(QWidget *parent)
	: OBSMainWindow(parent),
    ui(new Ui::GBSMainForm)
{
	ui->setupUi(this);
	//initLoginPanel(LoginPanelType::PASSWORD_LOGIN);
	//connect(this, &GBSMainForm::signalDestroyLoginWindow, this, &GBSMainForm::slotDestroyLoginWindow);
	


	setSizePolicy(
		QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint |
		       Qt::WindowMaximizeButtonHint |
		       Qt::WindowCloseButtonHint);
	setWindowTitle(OBSBasic::GetVendor() + " 直播软件登录");
	

	leftImage = new QLabel(this);
	QPixmap pixmapBG(":/gbs/images/gbs/login/login-default.png");
	leftImage->setPixmap(pixmapBG.scaled(739, 990, Qt::KeepAspectRatio));
	leftImage->setSizePolicy(QSizePolicy::Expanding,
				 QSizePolicy::Expanding);
	leftImage->setScaledContents(true); // 使图片内容跟随大小变化


	QHBoxLayout *loginWndCtlLayout = new QHBoxLayout;
	QPushButton *minimizeButton = new QPushButton("-", this);
	//QPushButton *maximizeButton = new QPushButton("+", this);
	QPushButton *closeButton = new QPushButton("x", this);

	minimizeButton->setFixedSize(60, 60);
	//maximizeButton->setFixedSize(60, 60);
	closeButton->setFixedSize(60, 60);

	loginWndCtlLayout->setSpacing(0);
	loginWndCtlLayout->addWidget(minimizeButton);
	//loginWndCtlLayout->addWidget(maximizeButton);
	loginWndCtlLayout->addWidget(closeButton);


	minimizeButton->setStyleSheet(
		"QPushButton {"
		"  border-radius: 10px;" // 设置圆角的半径为10像素
		"  font: 26px 'Times New Roman';" // 设置字体大小和类型
		"}"
		"QPushButton:hover {"
		"  background-color: #4EA8FF;" // 鼠标悬停时的背景颜色
		"  color: white;"              // 鼠标悬停时的文字颜色
		"}");

	//maximizeButton->setStyleSheet(
	//	"QPushButton {"
	//	"  border-radius: 10px;" // 设置圆角的半径为10像素
	//	"  font: 26px 'Times New Roman';" // 设置字体大小和类型
	//	"}"
	//	"QPushButton:hover {"
	//	"  background-color: #4EA8FF;" // 鼠标悬停时的背景颜色
	//	"  color: white;"              // 鼠标悬停时的文字颜色
	//	"}");

	closeButton->setStyleSheet(
		"QPushButton {"
		"  border-radius: 10px;" // 设置圆角的半径为10像素
		"  font: 26px 'Times New Roman';" // 设置字体大小和类型
		"}"
		"QPushButton:hover {"
		"  background-color: #4EA8FF;" // 鼠标悬停时的背景颜色
		"  color: white;"              // 鼠标悬停时的文字颜色
		"}");


	// 确保按钮在右上角
	loginWndCtlLayout->setAlignment(Qt::AlignTop | Qt::AlignRight);

	loginBizLayout = new QVBoxLayout;
	QVBoxLayout *rightLayout = new QVBoxLayout;



	// 添加控制按钮的布局
	rightLayout->addLayout(loginWndCtlLayout);



	normalLoginForm = new GBSNormalLoginForm(this);
	scanQRcodeForm = nullptr;
	authorizedCodeForm = nullptr;
	registerForm = nullptr;
	currentFrom = normalLoginForm;
	currentLoginType = NORMAL_LOGIN;
	loginBizLayout->addWidget(normalLoginForm);

	// 添加弹簧，确保按钮布局在窗口右上角
	rightLayout->addLayout(loginBizLayout);
	rightLayout->setStretch(0, 1);
	rightLayout->setStretch(1, 10);


	ui->horizontalLayout->addWidget(leftImage);
	ui->horizontalLayout->addLayout(rightLayout);

	// 设置布局拉伸比例，使组件按比例变化
	ui->horizontalLayout->setStretch(0, 1);
	ui->horizontalLayout->setStretch(1, 1);

	// 连接信号槽
	// connect(minimizeButton, &QPushButton::clicked, this,
	// 	&GBSLogin::showMinimized);
	//connect(maximizeButton, &QPushButton::clicked, this, [this]() {
	//	if (isMaximized()) {
	//		showNormal(); // 恢复正常大小
	//	} else {
	//		showMaximized(); // 最大化窗口
	//	}
	//});
	connect(closeButton, &QPushButton::clicked, this, &GBSMainForm::close);
	connect(normalLoginForm, &GBSNormalLoginForm::linkActivated, this, &GBSMainForm::onLinkActivated);
	connect(normalLoginForm, &GBSNormalLoginForm::loginTypeChanged, this, &GBSMainForm::onLoginTypeChanged);
	connect(normalLoginForm, &GBSNormalLoginForm::closeLoginWindow, this, &GBSMainForm::close);
	
	connect(normalLoginForm, &GBSNormalLoginForm::notifyLoginSuccess, this, &GBSMainForm::close);


}

GBSMainForm::~GBSMainForm()
{

    delete ui;
}


void GBSMainForm::onLinkActivated(const QString &link) {
	qDebug() << "onLinkActivated enter " << link;

	if (link == "register") {
		if (currentFrom != nullptr) {
			loginBizLayout->removeWidget(currentFrom);
			delete currentFrom;
			currentFrom = nullptr;
			registerForm = new GBSRegisterForm(this);
			loginBizLayout->addWidget(registerForm);
			connect(registerForm, &GBSRegisterForm::linkActivated, this, &GBSMainForm::onLinkActivated);
			currentFrom = registerForm;
		}

	} else if (link == "login") {
		if (currentFrom != nullptr) {

			loginBizLayout->removeWidget(currentFrom);
			delete currentFrom;
			currentFrom = nullptr;
			normalLoginForm = new GBSNormalLoginForm(this);
			loginBizLayout->addWidget(normalLoginForm);
			connect(normalLoginForm, &GBSNormalLoginForm::linkActivated, this, &GBSMainForm::onLinkActivated);
			connect(normalLoginForm, &GBSNormalLoginForm::loginTypeChanged, this, &GBSMainForm::onLoginTypeChanged);
			connect(normalLoginForm,
				&GBSNormalLoginForm::closeLoginWindow,
				this, &GBSMainForm::close);
			currentFrom = normalLoginForm;
		}
	}
	else if (link == "constrains") {

	} else if (link == "privacy") {

	}
}

void GBSMainForm::onLoginTypeChanged(int type){
	qDebug() << "onLoginTypeChanged enter " << type;

	switch (type) {
	case 0: {
		currentLoginType = LoginType::NORMAL_LOGIN;
		if (currentFrom != nullptr) {
			loginBizLayout->removeWidget(currentFrom);
			delete currentFrom;
			currentFrom = nullptr;
			normalLoginForm = new GBSNormalLoginForm(this);
			loginBizLayout->addWidget(normalLoginForm);
			connect(normalLoginForm, &GBSNormalLoginForm::linkActivated, this, &GBSMainForm::onLinkActivated);
			connect(normalLoginForm, &GBSNormalLoginForm::loginTypeChanged, this, &GBSMainForm::onLoginTypeChanged);
			connect(normalLoginForm,
				&GBSNormalLoginForm::closeLoginWindow, this,
				&GBSMainForm::close);
			currentFrom= normalLoginForm;
		}
		break;
	}
	case 1: {
		currentLoginType = LoginType::AUTHORIZED_LOGIN;
		if (currentFrom != nullptr) {
			loginBizLayout->removeWidget(currentFrom);
			delete currentFrom;
			currentFrom = nullptr;
			authorizedCodeForm = new GBSAuthorizedCodeForm(this);
			loginBizLayout->addWidget(authorizedCodeForm);
			connect(authorizedCodeForm, &GBSAuthorizedCodeForm::linkActivated, this, &GBSMainForm::onLinkActivated);
			connect(authorizedCodeForm, &GBSAuthorizedCodeForm::loginTypeChanged, this, &GBSMainForm::onLoginTypeChanged);

			currentFrom= authorizedCodeForm;
		}
		break;
	}
	case 2: {
		currentLoginType = LoginType::QRCODE_LOGIN;
		if (currentFrom != nullptr) {
			loginBizLayout->removeWidget(currentFrom);
			delete currentFrom;
			currentFrom = nullptr;
			scanQRcodeForm = new GBSQRCodeLoginForm(this);
			loginBizLayout->addWidget(scanQRcodeForm);
			connect(scanQRcodeForm, &GBSQRCodeLoginForm::linkActivated, this, &GBSMainForm::onLinkActivated);
			connect(scanQRcodeForm, &GBSQRCodeLoginForm::loginTypeChanged, this, &GBSMainForm::onLoginTypeChanged);

			currentFrom = scanQRcodeForm;

		}
		break;
	}
	}
}

 config_t *GBSMainForm::Config() const
 {
 	return nullptr;
 }

 void GBSMainForm::OBSInit() {
 	show();

 }

 int GBSMainForm::GetProfilePath(char *path, size_t size, const char *file) const
 {
 	return 0;
 }

void GBSMainForm::initLoginPanel(LoginType type) {
	if(type == LoginType::QRCODE_LOGIN) {
		initQRCodeLogin();
	} else if (type == LoginType::AUTHORIZED_LOGIN) {
		initPasswordLogin();
	} else if (type == LoginType::QRCODE_LOGIN) {
		initEmailLogin();
	} else {
		qDebug() << "Not support login type.";
	}


}
void GBSMainForm::initEmailLogin() {

	// // 使用 HTML 进行文本的颜色和背景设置
	// QString welcomeText = R"(
	// <span style='color: #000000;'>欢迎来到</span><br>
	// <span style='color: #2667FE;'>国内外矩阵算力人工智分发平台</span>
	// )";
	// ui->welcomeBanner->setText(welcomeText);
	// ui->welcomeBanner->setStyleSheet("font-size: 24px;");  // 调整字体大小


	// QString emailPlaceHolderText = R"(邮箱)";
	// ui->inputEmail->setPlaceholderText(emailPlaceHolderText);
	// ui->inputEmail->setStyleSheet("QLineEdit{border-width:0;border-style:outset}");

	// QString verifyCodePlaceHolderText = R"(请输入验证码)";
	// ui->inputVerifyCode->setPlaceholderText(verifyCodePlaceHolderText);
	// ui->inputVerifyCode->setStyleSheet("QLineEdit{border-width:0;border-style:outset}");


	// QString getDynVerifyCodeText = R"(获取动态密码)";
	// ui->getDynVerifyCode->setPlaceholderText(getDynVerifyCodeText);
	// ui->getDynVerifyCode->setStyleSheet("QLineEdit{border-width:0;border-style:outset}");

	// QString loginText = R"(登录账号)";
	// ui->btnLoginAccount->setText(loginText);
	// ui->btnLoginAccount->setStyleSheet("background-color: #2667FE;");

 //        QString loginWithPasswordText = R"(
 //        <p style="font-size: 16px;">
 //            <span style="color: black;">账号密码</span>
 //            <span style="color: #2667FE;">登录</span>
 //        </p>)";
 //        ui->loginWithPassword->setText(loginWithPasswordText);

 //        QString useQRCodeLoginText = R"(
 //        <p style="font-size: 16px;">
 //            <span style="color: black;">切换二维码</span>
 //            <span style="color: #2667FE;">登录-></span>
 //        </p>)";
 //        ui->useQRCodeLogin->setText(useQRCodeLoginText);

 //        QString agreeWithLisencesText = R"(
 //        <p style="font-size: 12px;">
 //            <span style="color: black;">已阅读并同意《</span>
 //            <span style="color: #2667FE;">用户协议</span>
 //            <span style="color: #black;">》＆《</span>
 //            <span style="color: #2667FE;">隐私政策</span>
 //            <span style="color: #black;">》</span>
 //        </p>)";


 //        ui->agreeWithLisences->setText(agreeWithLisencesText);
}
void GBSMainForm::initPasswordLogin() {
	// 使用 HTML 进行文本的颜色和背景设置
	// QString welcomeText = R"(
	// <span style='color: #000000;'>欢迎来到</span><br>
	// <span style='color: #2667FE;'>国内外矩阵算力人工智分发平台</span>
	// )";
	// ui->welcomeBanner->setText(welcomeText);
	// ui->welcomeBanner->setStyleSheet("font-size: 24px;");  // 调整字体大小


	// QString emailPlaceHolderText = R"(输入账号)";
	// ui->inputEmail->setStyleSheet("QLineEdit{border-width:0;border-style:outset}");
	// ui->inputEmail->setPlaceholderText(emailPlaceHolderText);
	// ui->inputEmail->setGeometry(20, 10, 270, 22);



	// ui->accountInvalid->setVisible(false);

	// QString verifyCodePlaceHolderText = R"(输入密码)";
	// ui->inputVerifyCode->setPlaceholderText(verifyCodePlaceHolderText);
	// ui->inputVerifyCode->setStyleSheet("QLineEdit{border-width:0;border-style:outset}");
	// ui->inputVerifyCode->setGeometry(20, 90, 270, 22);

	// ui->getDynVerifyCode->setVisible(false);

	// QString loginText = R"(登录账号)";
	// ui->btnLoginAccount->setText(loginText);
	// ui->btnLoginAccount->setStyleSheet("QPushButton{background-color:#2667FE;border:1px;border-radius:50px;}"
	// 				     "QPushButton:hover{border:1px;border-radius:50px;}"
	// 				     "QPushButton:pressed{border:1px;border-radius:50px;}");


 //        QString loginWithPasswordText = R"(
 //        <p style="font-size: 16px;">
 //            <span style="color: black;">验证码</span>
 //            <span style="color: #2667FE;">登录</span>
 //        </p>)";
 //        ui->loginWithPassword->setText(loginWithPasswordText);


 //        QString useQRCodeLoginText = R"(
 //        <p style="font-size: 16px;">
 //            <span style="color: black;">切换二维码</span>
 //            <span style="color: #2667FE;">登录-></span>
 //        </p>)";
 //        ui->useQRCodeLogin->setText(useQRCodeLoginText);

 //        QString agreeWithLisencesText = R"(
 //        <p style="font-size: 12px;">
 //            <span style="color: black;">已阅读并同意《</span>
 //            <span style="color: #2667FE;">用户协议</span>
 //            <span style="color: #black;">》＆《</span>
 //            <span style="color: #2667FE;">隐私政策</span>
 //            <span style="color: #black;">》</span>
 //        </p>)";


 //        ui->agreeWithLisences->setText(agreeWithLisencesText);
}
void GBSMainForm::initQRCodeLogin() {

}



void GBSMainForm::slotDestroyLoginWindow() {
	qDebug() << "slot catched.";
	mainWindow->close();
}

void GBSMainForm::setMainWindow(QMainWindow *window) {
	mainWindow = window;
}




void GBSMainForm::mousePressEvent(QMouseEvent *event)
{
	// 记录鼠标按下的位置
	if (event->button() == Qt::LeftButton) {
		m_dragging = true;
		m_dragStartPos = event->globalPosition().toPoint() - pos();
	}
}

void GBSMainForm::mouseMoveEvent(QMouseEvent *event)
{
	// 处理窗口拖动
	if (m_dragging) {
		move(event->globalPosition().toPoint() - m_dragStartPos);
	}
}

void GBSMainForm::mouseReleaseEvent(QMouseEvent *event)
{
	// 释放鼠标，停止拖动
	if (event->button() == Qt::LeftButton) {
		m_dragging = false;
	}
}
void GBSMainForm::wheelEvent(QWheelEvent *event)
{
	if (event->modifiers() & Qt::ControlModifier) {
		int delta = event->angleDelta().y();
		if (delta > 0) {
			resize(width() * 1.1, height() * 1.1);
		} else if (delta < 0) {
			resize(width() * 0.9, height() * 0.9);
		}
	}
}
