#include "gbsregisterform.h"
#include "ui_gbsregisterform.h"

GBSRegisterForm::GBSRegisterForm(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GBSRegisterForm)
{
	ui->setupUi(this);

	ui->imgBanner->setStyleSheet("border-image:url(:/gbs/images/gbs/login/gbs-banner.png)");
	QString registerMessage = R"(
	    <p style="font-size: 18px; text-align: center;">
		<span style="color: #000000;">注册</span>
	    </p>
	)";
	ui->lblRegister->setText(registerMessage);


	ui->btnRegisterGBS->setStyleSheet(
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

        QString existAccountMessage = R"(
            <p style="font-size: 15px; text-align: center;">
                <span style="color: #9CA4AB;">已经有账号? </span>
                <a href="login" style="color: #EB3F5E; text-decoration: none;">登录</a>
            </p>
        )";
        ui->lblAccountExisted->setText(existAccountMessage);
        ui->lblAccountExisted->setTextFormat(Qt::RichText);
        ui->lblAccountExisted->setTextInteractionFlags(Qt::TextBrowserInteraction);
        ui->lblAccountExisted->setOpenExternalLinks(false);
        connect(ui->lblAccountExisted, &QLabel::linkActivated, this, &GBSRegisterForm::onLinkActivated);


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

	connect(ui->lblConstraintAndPrivacy, &QLabel::linkActivated, this, &GBSRegisterForm::onLinkActivated);

}


GBSRegisterForm::~GBSRegisterForm()
{
	delete ui;
}


void GBSRegisterForm::onQRcodeLogin() {
	qDebug() << "onQRcodeLogin click ";
	emit loginTypeChanged(2);
}

void GBSRegisterForm::onAuthorizedLogin() {
	qDebug() << "onAuthorizedLogin click ";
	emit loginTypeChanged(1);
}

void GBSRegisterForm::onLinkActivated(const QString &link) {
	emit linkActivated(link);

}
void GBSRegisterForm::onLoginTypeChanged(int type) {
	emit loginTypeChanged(type);
}

