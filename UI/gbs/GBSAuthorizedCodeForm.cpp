#include "gbsauthorizedcodeform.h"
#include "ui_gbsauthorizedcodeform.h"
#include "gbs/common/GBSHttpClient.h"
#include "GBSMainBizWindow.h"
#include "gbs/common/QBizLogger.h"
#include "gbs/bizWidgets/GBSMsgDialog.h"
#include "gbs/common/QBizLogger.h"
#include "gbs/bizWidgets/GBSMsgDialog.h"
#include "gbs/common/QIniFile.h"
#include <QTextBrowser>
#include <QDialogButtonBox>

#include <QStyle>


GBSAuthorizedCodeForm::GBSAuthorizedCodeForm(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GBSAuthorizedCodeForm)
{
	ui->setupUi(this);
	passwordProtecter = std::make_unique<XORPasswordProtecter>("asdfghjkl;'\\！@#￥");
	ui->imgBanner->setStyleSheet("border-image:url(:/gbs/images/gbs/login/gbs-banner.png)");
	ui->btnScanQRodeLogin->setStyleSheet("border-image:url(:/gbs/images/gbs/login/scan-qrcode-login.png)");
	ui->btnAuthorizeCodeLogin->setStyleSheet("border-image:url(:/gbs/images/gbs/login/account-password-login.png)");

        QString welcomeMessage = R"(
            <p style="font-size: 32px; text-align: center;">
                <span style="color: red;">Hi！</span>
                <span style="color: black;">欢迎来到算力分发平台</span>
            </p>
        )";
	std::unique_ptr<IniSettings> iniFile = std::make_unique<IniSettings>("gbs.ini");
	QString emailValue;
	QString passwordValue;
	QString loginToken;
	emailValue = iniFile->value("User", "login.Username", "unknown").toString();
	passwordValue = iniFile->value("User", "login.Smscode", "unknown").toString();
	loginToken = iniFile->value("User", "login.Token", "unknown").toString();

	if ((emailValue != "unknown") && (passwordValue != "unknown")) {
		QString decPassword = passwordProtecter->decrypt(passwordValue.toUtf8());
		ui->leEmail->setText(emailValue);
		//ui->leSmsCode->setText(decPassword);
		ui->radioButton->setChecked(true);
	}

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

        connect(ui->lblNoAccount, &QLabel::linkActivated, this, &GBSAuthorizedCodeForm::onLinkActivated);


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

	connect(ui->lblConstraintAndPrivacy, &QLabel::linkActivated, this, &GBSAuthorizedCodeForm::onLinkActivated);


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
		"   background-image: url(:/gbs/images/gbs/login/account-password-login.png);"
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
		"   background-image: url(:/gbs/images/gbs/login/account-password-login.png);"
		"   background-repeat: no-repeat;"
		"   background-position: center;"
		"}"
		);


	connect(ui->btnScanQRodeLogin, &QPushButton::clicked, this, &GBSAuthorizedCodeForm::onQRcodeLogin);
	connect(ui->btnAuthorizeCodeLogin, &QPushButton::clicked, this, &GBSAuthorizedCodeForm::onAuthorizedLogin);
	connect(ui->btnLoginGBS,&QPushButton::clicked, this, &GBSAuthorizedCodeForm::onLoginGBS);
	
	GBSHttpClient::getInstance()->registerHandler(this);
	if (loginToken != "unknown") {
		GBSHttpClient::getInstance()->setRecordedToken(loginToken.toStdString());
		GBSHttpClient::getInstance()->getUserInfo();
	}
}

GBSAuthorizedCodeForm::~GBSAuthorizedCodeForm()
{
	GBSHttpClient::getInstance()->unRegisterHandler(this);
	delete ui;
}


void GBSAuthorizedCodeForm::onQRcodeLogin() {
	qDebug() << "onQRcodeLogin click ";
	emit loginTypeChanged(2);
}

void GBSAuthorizedCodeForm::onAuthorizedLogin() {
	qDebug() << "onAuthorizedLogin click ";
	emit loginTypeChanged(0);
}

void GBSAuthorizedCodeForm::onLinkActivated(const QString &link) {
	emit linkActivated(link);

}
void GBSAuthorizedCodeForm::onLoginTypeChanged(int type) {
	emit loginTypeChanged(type);
}

void GBSAuthorizedCodeForm::onLoginGBS() {

	ui->btnLoginGBS->setDisabled(true);
	QString email = ui->leEmail->text();
	QString smsCode = ui->leSmsCode->text();

	if ((email.isEmpty()) || (smsCode.isEmpty())) {

		QWidget *widget = new QWidget;
		QVBoxLayout *layout = new QVBoxLayout(widget);
		QLabel *label = new QLabel("用户名或者密码不能为空！");
		layout->setAlignment(Qt::AlignCenter); // 整体内容居中

		layout->addWidget(label);

		GBSMsgDialog *dialog = new GBSMsgDialog("提示", layout, this);
		dialog->exec();
		ui->btnLoginGBS->setDisabled(false);
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
		ui->btnLoginGBS->setDisabled(false);
		return;
	}

	GBSHttpClient::getInstance()->srsAccountLoginV2(email.toUtf8().constData(), "", smsCode.toUtf8().constData(),
							"多多客", 4);
}

void GBSAuthorizedCodeForm::onUserInfo(const GBSUserInfo *info)
{
	QMetaObject::invokeMethod(
		this,
		[this]() {
			QLogD("Start to switch Login to MainBiz in AuthorizedCode Login");
			GBSMainBizWindow *gbsMainBizWindow = new GBSMainBizWindow();
			gbsMainBizWindow->setGeometry(
				QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, gbsMainBizWindow->size(),
						    QGuiApplication::primaryScreen()->availableGeometry()));
			gbsMainBizWindow->show();
			emit notifyLoginSuccess();
		},
		Qt::QueuedConnection);
}

void GBSAuthorizedCodeForm::onLoginResult(const int result, const std::string token)
{

	if (result == 0) {
		QMetaObject::invokeMethod(
			this,
			[this, token]() {
				std::unique_ptr<IniSettings> iniFile = std::make_unique<IniSettings>("gbs.ini");
				QString autoLoginEnable = iniFile->value("User", "login.Type", "unknown").toString();
				QString emailValue;
				QString passwordValue;

				emailValue = iniFile->value("User", "login.Username", "unknown").toString();
				passwordValue = iniFile->value("User", "login.Smscode", "unknown").toString();

				QString email = ui->leEmail->text();
				QString password = ui->leSmsCode->text();
				if (((emailValue == "unknown") || (passwordValue == "unknown")) ||
				    ((email != emailValue) && (password != passwordValue))) {
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
					//GBSMsgDialog *dialog = new GBSMsgDialog("提示", layout, this);
					//connect(confirm, &QPushButton::clicked, dialog, [dialog, token, this]() {
						QByteArray encPassword = passwordProtecter->encrypt(
							this->ui->leSmsCode->text().toUtf8());
						std::unique_ptr<IniSettings> iniFile =
							std::make_unique<IniSettings>("gbs.ini");
						iniFile->setValue("User", "login.Type", "smscode");
						iniFile->setValue("User", "login.Username", this->ui->leEmail->text());
						iniFile->setValue("User", "login.Smscode", QString(encPassword));
						iniFile->setValue("User", "login.Token", QString::fromStdString(token));
					//	dialog->accept();
					//});
					//connect(cancel, &QPushButton::clicked, dialog, &QDialog::reject);

					//dialog->exec();
				}

				QLogD("Start to switch Login to MainBiz");
				GBSMainBizWindow *gbsMainBizWindow = new GBSMainBizWindow();
				gbsMainBizWindow->setGeometry(
					QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, gbsMainBizWindow->size(),
							    QGuiApplication::primaryScreen()->availableGeometry()));
				gbsMainBizWindow->show();
				emit notifyLoginSuccess();
			},
			Qt::QueuedConnection);

	} else if (result == -1) {
		QMetaObject::invokeMethod(this, [this]() {
			QWidget *widget = new QWidget;
			QVBoxLayout *layout = new QVBoxLayout(widget);

			// 创建标签
			QLabel *label = new QLabel("登录失败");
			QLabel *subLabel = new QLabel("对不起，服务器返回错误");

			// 设置标签样式
			label->setStyleSheet("QLabel {"
					     "font-size: 16px;"
					     "color: black;"
					     "}");
			subLabel->setStyleSheet("QLabel {"
						"font-size: 12px;"
						"color: black;"
						"}");
			layout->addSpacing(40);
			// 添加标签到布局，确保水平居中
			layout->addWidget(label, 0, Qt::AlignHCenter);
			layout->addSpacing(10); // 标签之间的间距
			layout->addWidget(subLabel, 0, Qt::AlignHCenter);

			// 添加底部弹性空间
			layout->addStretch();

			// 设置整体布局
			//layout->setContentsMargins(20, 20, 20, 20); // 设置边距
			layout->setAlignment(Qt::AlignVCenter); // 确保整体布局居中

			// 创建自定义对话框并显示
			GBSMsgDialog *dialog = new GBSMsgDialog("提示", layout, this);
			dialog->exec();

			ui->btnLoginGBS->setDisabled(false);
		});
	} else if (result == -2) {

	} else if (result == -3) {
		QMetaObject::invokeMethod(this, [this]() {
			QWidget *widget = new QWidget;
			QVBoxLayout *layout = new QVBoxLayout(widget);

			// 创建标签
			QLabel *label = new QLabel("登录失败");
			QLabel *subLabel = new QLabel("对不起，用户名或密码错误");

			// 设置标签样式
			label->setStyleSheet("QLabel {"
					     "font-size: 16px;"
					     "color: black;"
					     "}");
			subLabel->setStyleSheet("QLabel {"
						"font-size: 12px;"
						"color: black;"
						"}");
			layout->addSpacing(40);
			// 添加标签到布局，确保水平居中
			layout->addWidget(label, 0, Qt::AlignHCenter);
			layout->addSpacing(10); // 标签之间的间距
			layout->addWidget(subLabel, 0, Qt::AlignHCenter);

			// 添加底部弹性空间
			layout->addStretch();

			// 设置整体布局
			//layout->setContentsMargins(20, 20, 20, 20); // 设置边距
			layout->setAlignment(Qt::AlignVCenter); // 确保整体布局居中

			// 创建自定义对话框并显示
			GBSMsgDialog *dialog = new GBSMsgDialog("提示", layout, this);
			dialog->exec();
			ui->btnLoginGBS->setDisabled(false);
		});
	} else if (result == -5) {
		QLogE("Login token is expired or invalid");
	}
}

void GBSAuthorizedCodeForm::onAgreementInfo(std::string richText, int type)
{
	QMetaObject::invokeMethod(this, [richText, type, this]() {
		QTextBrowser *browser = new QTextBrowser;
		QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

		browser->setFixedSize(1440, 880);
		browser->setHtml(QString::fromStdString(richText));
		QDialog *dialog = new QDialog;
		if (type == 1) {
			dialog->setWindowTitle("用户协议");
		} else if (type == 2) {
			dialog->setWindowTitle("隐私政策");
		}

		QVBoxLayout *layout = new QVBoxLayout();
		dialog->setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

		layout->addWidget(browser);
		layout->addWidget(buttonBox);
		dialog->setLayout(layout);
		connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept); // 确认按钮
		connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject); // 取消按钮
		dialog->exec();
	});
}
