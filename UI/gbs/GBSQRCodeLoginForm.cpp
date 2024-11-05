#include "gbsqrcodeloginform.h"
#include "ui_gbsqrcodeloginform.h"

GBSQRCodeLoginForm::GBSQRCodeLoginForm(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GBSQRCodeLoginForm)
{
	ui->setupUi(this);
	ui->imgBanner->setStyleSheet("border-image:url(:/gbs/images/gbs/login/gbs-banner.png)");
	ui->btnScanQRodeLogin->setStyleSheet("border-image:url(:/gbs/images/gbs/login/scan-qrcode-login.png)");
	ui->btnAuthorizeCodeLogin->setStyleSheet("border-image:url(:/gbs/images/gbs/login/authorized-code-login.png)");


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

        connect(ui->lblNoAccount, &QLabel::linkActivated, this, &GBSQRCodeLoginForm::onLinkActivated);

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

	connect(ui->lblConstraintAndPrivacy, &QLabel::linkActivated, this, &GBSQRCodeLoginForm::onLinkActivated);


        QString openAppPromptMessage = R"(
            <p style="font-size: 15px; text-align: center;">
                <span style="color: #9CA4AB;">请打开</span>
                <a href="openAppHref" style="color: #EB3F5E; text-decoration: none;">国播出国</a>
                <span style="color: #9CA4AB;">扫码登录</span>
            </p>
        )";

	ui->lblOpenApp->setText(openAppPromptMessage);
	ui->lblOpenApp->setTextFormat(Qt::RichText);
	ui->lblOpenApp->setTextInteractionFlags(Qt::TextBrowserInteraction);
	ui->lblOpenApp->setOpenExternalLinks(false);


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

	ui->btnLoginGBS->setEnabled(false);

	connect(ui->btnScanQRodeLogin, &QPushButton::clicked, this, &GBSQRCodeLoginForm::onQRcodeLogin);
	connect(ui->btnAuthorizeCodeLogin, &QPushButton::clicked, this, &GBSQRCodeLoginForm::onAuthorizedLogin);
	connect(ui->btnLoginGBS,&QPushButton::clicked, this, &GBSQRCodeLoginForm::onLoginGBS);

}

GBSQRCodeLoginForm::~GBSQRCodeLoginForm()
{
	delete ui;
}


void GBSQRCodeLoginForm::onQRcodeLogin() {
	qDebug() << "onQRcodeLogin click ";
	emit loginTypeChanged(2);
}

void GBSQRCodeLoginForm::onAuthorizedLogin() {
	qDebug() << "onAuthorizedLogin click ";
	emit loginTypeChanged(1);
}

void GBSQRCodeLoginForm::onLinkActivated(const QString &link) {
	emit linkActivated(link);

}
void GBSQRCodeLoginForm::onLoginTypeChanged(int type) {
	emit loginTypeChanged(type);
}

void GBSQRCodeLoginForm::onLoginGBS() {

}
