#include "gbsnormalloginform.h"
#include "ui_gbsnormalloginform.h"
#include <QIcon>
#include <QPalette>

#include "gbs/common/GBSHttpClient.h"

#include "GBSMainBizWindow.h"
#include "gbs/common/WebSocketClient.h"
#include "gbs/common/QBizLogger.h"
#include "gbs/bizWidgets/GBSMsgDialog.h"
#include "gbs/common/QIniFile.h"

GBSNormalLoginForm::GBSNormalLoginForm(QWidget *parent) : OBSMainWindow(parent),
	  ui(new Ui::GBSNormalLoginForm)
{
	ui->setupUi(this);
	ui->imgBanner->setStyleSheet("border-image:url(:/gbs/images/gbs/login/gbs-banner.png)");
	ui->btnScanQRodeLogin->setStyleSheet("border-image:url(:/gbs/images/gbs/login/scan-qrcode-login.png)");
	ui->btnAuthorizeCodeLogin->setStyleSheet("border-image:url(:/gbs/images/gbs/login/authorized-code-login.png)");
	ui->lePassword->setEchoMode(QLineEdit::Password);

	std::unique_ptr<IniSettings> iniFile = std::make_unique<IniSettings>("gbs.ini");
	QString autoLoginEnable = iniFile->value("User", "login.Type", "unknown").toString();
	QString emailValue;
	QString passwordValue;
	
	emailValue = iniFile->value("User", "login.Username", "unknown").toString();
	passwordValue = iniFile->value("User", "login.Password", "unknown").toString();
	

	if ((emailValue != "unknown") && (passwordValue != "unknown")) {
		ui->leEmal->setText(emailValue);
		ui->lePassword->setText(passwordValue);
		ui->radioButton->setChecked(true);
	}

        QString welcomeMessage = R"(
            <p style="font-size: 32px; text-align: center;">
                <span style="color: red;">Hi！</span>
                <span style="color: black;">欢迎来到算力分发平台</span>
            </p>
        )";

	ui->lblWelcome->setText(welcomeMessage);
	ui->lblWelcome->setAlignment(Qt::AlignCenter); // 水平和垂直居中
	//ui->lblWelcome->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 允许 QLabel 扩展
	//ui->lblWelcome->setWordWrap(true); // 如果文字超过宽度，允许换行


        QString noAccountMessage = R"(
            <p style="font-size: 15px; text-align: center;">
                <span style="color: #9CA4AB;">没有账号? </span>
                <a href="register" style="color: #EB3F5E; text-decoration: none;">注册</a>
            </p>
        )";
        ui->lblNoAccount->setText(noAccountMessage);
        ui->lblNoAccount->setTextFormat(Qt::RichText);
        ui->lblNoAccount->setTextInteractionFlags(Qt::TextBrowserInteraction);
        ui->lblNoAccount->setOpenExternalLinks(false);

        connect(ui->lblNoAccount, &QLabel::linkActivated, this, &GBSNormalLoginForm::onLinkActivated);

        QString agreeWithLisencesText = R"(
        <p style="font-size: 12px;">
            <span style="color: black;">已阅读并同意《</span>
             <a href="constrains" style="color: #00C566; text-decoration: none;">用户协议</a>
            <span style="color: #black;">》＆《</span>
             <a href="privacy" style="color: #00C566; text-decoration: none;">隐私政策</a>
            <span style="color: #black;">》</span>
        </p>)";

	ui->lblConstraintAndPrivacy->setText(agreeWithLisencesText);
	ui->lblConstraintAndPrivacy->setTextFormat(Qt::RichText);
	ui->lblConstraintAndPrivacy->setTextInteractionFlags(Qt::TextBrowserInteraction);
	ui->lblConstraintAndPrivacy->setOpenExternalLinks(false);

	connect(ui->lblConstraintAndPrivacy, &QLabel::linkActivated, this, &GBSNormalLoginForm::onLinkActivated);

	// 连接 QLabel 的 linkActivated 信号到 lambda 表达式
	// connect(ui->lblConstraintAndPrivacy, &QLabel::linkActivated, this, [this](const QString &link) {
	// 	qDebug() << "onLinkActivated enter " << link;

	// 	if (link == "register") {
	// 	} else if (link == "constrains") {

	// 	} else if (link == "privacy") {

	// 	}
	// });


	// 设置样式表，添加背景图片
	ui->btnScanQRodeLogin->setStyleSheet(
		"QPushButton {"
		"   background-image: url(:/gbs/images/gbs/login/scan-qrcode-login.png);"
		"   background-repeat: no-repeat;"
		"   background-position: center;"
		"   color: white;"
		"   border: none;" // 无边框
		"   border-radius: 5px;" // 圆角
		"   font-size: 16px;"
		"   padding: 0;" // 不添加内边距
		"}"
		"QPushButton:hover {"
		"   background-color: #F9F9F9;"
		"}"
		"QPushButton:pressed {"
		"   background-color: #D1D8DD;" // 按下时背景颜色
		"   padding-left: 3px;"  // 向左移动 3px
		"   padding-top: 3px;"    // 向上移动 3px
		"   background-image: url(:/gbs/images/gbs/login/scan-qrcode-login.png);"
		"   background-repeat: no-repeat;"
		"   background-position: center;"
		"}"
		);


	ui->btnAuthorizeCodeLogin->setStyleSheet(
		"QPushButton {"
		"   background-image: url(:/gbs/images/gbs/login/authorized-code-login.png);"
		"   background-repeat: no-repeat;"
		"   background-position: center;"
		"   color: white;"
		"   border: none;" // 无边框
		"   border-radius: 5px;" // 圆角
		"   font-size: 16px;"
		"   padding: 0;" // 不添加内边距
		"}"
		"QPushButton:hover {"
		"   background-color: #F9F9F9;"
		"}"
		"QPushButton:pressed {"
		"   background-color: #D1D8DD;" // 按下时背景颜色
		"   padding-left: 3px;"  // 向左移动 3px
		"   padding-top: 3px;"    // 向上移动 3px
		"   background-image: url(:/gbs/images/gbs/login/authorized-code-login.png);"
		"   background-repeat: no-repeat;"
		"   background-position: center;"
		"}"
		);

	ui->btnForgetPassword->setStyleSheet(
		"QPushButton {"
		"   background-color: transparent;"
		"   border: none;" // 无边框
		"   border-radius: 5px;" // 圆角
		"   font-size: 14px;"
		"   text-align: center;"
		"}"
		"QPushButton:pressed {"
		"   background-color: #F9F9F9;" // 按下状态背景颜色
		"   padding-left: 1px;  /* 向左移动 3px */"
		"   padding-top: 1px;    /* 向上移动 3px */"
		"}"
		);



	ui->btnLoginGBS->setStyleSheet(
		"QPushButton {"
		"   background-color: #1B2846;" // 正常状态背景颜色
		"   color: white;"
		"   border: none;" // 无边框
		"   border-radius: 5px;" // 圆角
		"   font-size: 16px;"
		"}"
		"QPushButton:pressed {"
		"   background-color: #1B2846;" // 按下状态背景颜色
		"   padding-left: 1px;  /* 向左移动 3px */"
		"   padding-top: 1px;    /* 向上移动 3px */"
		"}"
		);
	connect(ui->btnScanQRodeLogin, &QPushButton::clicked, this, &GBSNormalLoginForm::onQRcodeLogin);
	connect(ui->btnAuthorizeCodeLogin, &QPushButton::clicked, this, &GBSNormalLoginForm::onAuthorizedLogin);
	connect(ui->btnForgetPassword, &QPushButton::clicked, this, &GBSNormalLoginForm::onPasswordForgot);
	connect(ui->btnLoginGBS,&QPushButton::clicked, this, &GBSNormalLoginForm::onLoginGBS);

	GBSHttpClient::getInstance()->registerHandler(this);
	GBSHttpClient::getInstance()->getUserInfo();
}

void GBSNormalLoginForm::onLinkActivated(const QString &link) {
	emit linkActivated(link);

}

void GBSNormalLoginForm::onLoginTypeChanged(int type) {
	emit loginTypeChanged(type);
}


void GBSNormalLoginForm::onQRcodeLogin() {
	qDebug() << "onQRcodeLogin click ";
	emit loginTypeChanged(2);
}

void GBSNormalLoginForm::onAuthorizedLogin() {
	qDebug() << "onAuthorizedLogin click ";
	emit loginTypeChanged(1);
}

void GBSNormalLoginForm::onPasswordForgot() {
	qDebug() << "onPasswordForgot click ";
}
/*

*/
void GBSNormalLoginForm::onLoginGBS() {
	qDebug() << "onLoginGBS click ";


	QString email = ui->leEmal->text();
	QString password = ui->lePassword->text();

	if ((email.isEmpty()) || (password.isEmpty())) {
		
		QWidget *widget = new QWidget;
		QVBoxLayout *layout = new QVBoxLayout(widget);
		QLabel *label = new QLabel("用户名或者密码不能为空！");
		layout->setAlignment(Qt::AlignCenter); // 整体内容居中

		layout->addWidget(label);
		

		GBSMsgDialog *dialog = new GBSMsgDialog("提示", layout, this);
		dialog->exec();
		return;
	}

	if (ui->radioButton->isChecked() == false) {
		QWidget *widget = new QWidget;
		QVBoxLayout *layout = new QVBoxLayout(widget);
		QLabel *label = new QLabel("请选择同意《用户协议》&《隐私政策》！");
		layout->setAlignment(Qt::AlignCenter); // 整体内容居中

		layout->addWidget(label);

		GBSMsgDialog *dialog = new GBSMsgDialog("提示", layout, this);
		dialog->exec();
		return;
	}



	GBSHttpClient::getInstance()->loginWithCheckVersion(
	email.toUtf8().constData(), password.toUtf8().constData(), 5,"多多客");



	
}
void GBSNormalLoginForm::onLoginResult(const int result) {

	if (result == 0) {
		QMetaObject::invokeMethod(
			this,
			[this]() {

				std::unique_ptr<IniSettings> iniFile = std::make_unique<IniSettings>("gbs.ini");
				QString autoLoginEnable = iniFile->value("User", "login.Type", "unknown").toString();
				QString emailValue;
				QString passwordValue;
				
				emailValue = iniFile->value("User", "login.Username", "unknown").toString();
				passwordValue = iniFile->value("User", "login.Password", "unknown").toString();
				

				if ((emailValue== "unknown") || (passwordValue == "unknown")) {
					QWidget *widget = new QWidget;
					QVBoxLayout *layout = new QVBoxLayout(widget);
					QHBoxLayout *layout1 = new QHBoxLayout(widget);
					QLabel *label = new QLabel("是否记住你登录方式，下次方便登录");
					QHBoxLayout *layout2 = new QHBoxLayout(widget);
					QPushButton *confirm = new QPushButton("是");
					QPushButton *cancel = new QPushButton("否");
					confirm->setStyleSheet("QPushButton {"
								"   background-color: #8F92A1;"
								"   border: #000000;"     // 无边框
								"   border-radius: 10px;" // 圆角
								"   font-size: 14px;"
								"   text-align: center;" // 左对齐文本
								"}"

								"QPushButton:hover {"
								"   background-color: #1B2846;"
								"   color: white;" // 悬停状态字体颜色

								"}"
								"QPushButton:pressed {"
								"   background-color: #F9F9F9;" // 按下状态背景颜色
								"   padding-left: 1px;  /* 向左移动 3px */"
								"   padding-top: 1px;    /* 向上移动 3px */"
								"   color: white;" // 按下状态字体颜色
								"}");
					cancel->setStyleSheet("QPushButton {"
								"   background-color: #8F92A1;"
								"   border: #000000;"     // 无边框
								"   border-radius: 10px;" // 圆角
								"   font-size: 14px;"
								"   text-align: center;" // 左对齐文本
								"}"

								"QPushButton:hover {"
								"   background-color: #1B2846;"
								"   color: white;" // 悬停状态字体颜色

								"}"
								"QPushButton:pressed {"
								"   background-color: #F9F9F9;" // 按下状态背景颜色
								"   padding-left: 1px;  /* 向左移动 3px */"
								"   padding-top: 1px;    /* 向上移动 3px */"
								"   color: white;" // 按下状态字体颜色
								"}");

					layout2->addWidget(confirm);
					layout2->addWidget(cancel);
					layout->setAlignment(Qt::AlignCenter); // 整体内容居中
					layout1->addWidget(label);
					layout->addLayout(layout1);
					layout->addSpacing(40);
					layout->addLayout(layout2);
					GBSMsgDialog *dialog = new GBSMsgDialog("提示", layout, this);
					connect(confirm, &QPushButton::clicked, dialog, [dialog, this]() {
						std::unique_ptr<IniSettings> iniFile = std::make_unique<IniSettings>("gbs.ini");
						iniFile->setValue("User", "login.Type", "normal");
						iniFile->setValue("User", "login.Username", this->ui->leEmal->text());
						iniFile->setValue("User", "login.Password", this->ui->lePassword->text());
						dialog->accept();
						});
					connect(cancel, &QPushButton::clicked, dialog, &QDialog::reject);

					dialog->exec();
				}

				QLogD("Start to switch Login to MainBiz");
				GBSMainBizWindow *gbsMainBizWindow =
					new GBSMainBizWindow();
				gbsMainBizWindow->show();
				emit notifyLoginSuccess();

			},
			Qt::QueuedConnection);

	} else if (result == -1) {

	} else if (result == -2) {

	} else if (result == -3) {
		QMetaObject::invokeMethod(this, [this]() {
			QWidget *widget = new QWidget;
			QVBoxLayout *layout = new QVBoxLayout(widget);
			QLabel *label = new QLabel("登录失败");
			QLabel *subLabel = new QLabel("对不起，密码错误");
			//label->setFixedSize(320, 80);
			label->setStyleSheet("QLabel {"
					    "font-size: 16px;"
					    "color: black;"
					    "}");
			subLabel->setStyleSheet("QLabel {"
					     "font-size: 12px;"
					     "color: black;"
					     "}");
			layout->addWidget(label);
			layout->addSpacing(40);
			layout->addWidget(subLabel);
			layout->setAlignment(Qt::AlignCenter); // 整体内容居中
			GBSMsgDialog *dialog = new GBSMsgDialog("提示", layout, this);
			dialog->exec();
		});
	}

}
void GBSNormalLoginForm::onRtmpPushUrl(const std::string url) {}
void GBSNormalLoginForm::onPullRtmpUrl(const std::string url) {}
void GBSNormalLoginForm::onUserInfo(const GBSUserInfo *info) {}
void GBSNormalLoginForm::onUserFileDownLoad(const std::string &path, int type) {}
void GBSNormalLoginForm::onRoomInfos(std::list<GBSRoomInfo> &info) {

}
void GBSNormalLoginForm::onRoomInfo(GBSRoomInfo *info) {}
void GBSNormalLoginForm::onQRcodeInfo(std::string no, std::string url, int status) {}

config_t *GBSNormalLoginForm::Config() const
{
	return nullptr;
}
void GBSNormalLoginForm::OBSInit() {
	show();
}
int GBSNormalLoginForm::GetProfilePath(char *path, size_t size, const char *file) const
{
	return 0;
}
GBSNormalLoginForm::~GBSNormalLoginForm()
{
	GBSHttpClient::getInstance()->unRegisterHandler(this);
	delete ui;
}
