#include "GBSBizLiveGuarderAuth.h"
#include "ui_GBSBizLiveGuarderAuth.h"
#include "GBSBizLiveGuarder.h"
#include "gbs/common/GBSHttpClient.h"
#include "gbs/GBSMainCollector.h"
#include "gbs/dto/GBSLiveAccountInfo.h"
#include "gbs/common/QBizLogger.h"
#include <QList>
#include<QLineEdit>
#define AUTH_CODE_NUM 6
GBSBizLiveGuarderAuth::GBSBizLiveGuarderAuth(QWidget *parent)
	: QWidget(parent),
      ui(new Ui::GBSBizLiveGuarderAuth)
{
	ui->setupUi(this);

    qlists.push_back(ui->lePassword0);
    qlists.push_back(ui->lePassword1);
    qlists.push_back(ui->lePassword2);
    qlists.push_back(ui->lePassword3);
    qlists.push_back(ui->lePassword4);
    qlists.push_back(ui->lePassword5);

    for (int i = 0; i < AUTH_CODE_NUM; i++) {
        qlists[i]->setMaxLength(1);
        qlists[i]->setMaxLength(1); // 限制每个输入框最多输入1个字符
        qlists[i]->setAlignment(Qt::AlignCenter); // 居中显示
        // 设置样式表，使其看起来像一个圆形框
        qlists[i]->setStyleSheet(R"(
                QLineEdit {
                    border: 1px solid #E9EBED;  /* 虚线边框 */
                    border-radius: 25px;           /* 圆角，半径等于宽高一半 */
                    background-color: #F9F9F9;     /* 背景颜色 */
                    font-size: 24px;               /* 字体大小 */
                    size:24px;
                    color: gray;                   /* 字体颜色 */
            center:true;
                }
                QLineEdit:focus {
                    border: 2px solid green;       /* 焦点状态边框 */
                }
            )");

        // 连接 textChanged 信号，每次输入后自动跳转到下一个框
        connect(qlists[i], &QLineEdit::textChanged, this, [=]() {
            if (qlists[i]->text().length() == 1) {
                focusNextInput(i);
            }
        });
    }
    qlists[0]->setFocus();


    QString resetPassword = R"(
        <p style="font-size: 18px;">
            <span style="color: black;">忘记了？</span>
             <a href="constrains" style="color: #00C566; text-decoration: none;">重置</a>
        </p>)";
    ui->lblResetPassword->setText(resetPassword);
    ui->lblResetPassword->setTextFormat(Qt::RichText);
    ui->lblResetPassword->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->lblResetPassword->setOpenExternalLinks(false);

    connect(this, &GBSBizLiveGuarderAuth::enterGuarderCtrl, reinterpret_cast<GBSBizLiveGuarder *>(parent), &GBSBizLiveGuarder::enterGuarderCtrl);

    connect(ui->pushButton, &QPushButton::clicked, this, &GBSBizLiveGuarderAuth::sendEnterCtrl);
    GBSHttpClient::getInstance()->registerHandler(this);
}

void GBSBizLiveGuarderAuth::sendEnterCtrl() {
	GBSLiveAccountInfo account = GBSMainCollector::getInstance()->getAccountInfo();
	GBSHttpClient::getInstance()->enterControlV2(getInputPassword().toUtf8().constData(), account.getId());
}

GBSBizLiveGuarderAuth::~GBSBizLiveGuarderAuth()
{
	GBSHttpClient::getInstance()->unRegisterHandler(this);
	qlists.clear();
	delete ui;
}

void GBSBizLiveGuarderAuth::focusNextInput(int currentIndex) {
    qlists[(currentIndex+1) % 6]->setFocus(); // 跳到下一个输入框
	if (currentIndex == 5) {
	    ui->pushButton->setFocus();
	}
}

void GBSBizLiveGuarderAuth::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
		bool valid = true;
		for (auto le : qlists) {
			if (le->text().isEmpty()) {
				valid = false;
			}
		}
		if (valid) {
			sendEnterCtrl();
		}
	}
    QWidget::keyPressEvent(event); // 调用父类的默认行为
}


QString GBSBizLiveGuarderAuth::getPresetPassword() {
    return presetPassword;
}
QString GBSBizLiveGuarderAuth::getInputPassword() {
	inputPassword.clear();
	for (int i = 0; i < qlists.count(); i++) {
		inputPassword += qlists.at(i)->text();
	}
	qDebug() << "inputPassword " << inputPassword;

    return inputPassword;
}
void GBSBizLiveGuarderAuth::setPresetPassword(QString password) {
    presetPassword = password;
}

void GBSBizLiveGuarderAuth::onEnterGuardCtrl(int result) {
	if (result == 1) {
		emit enterGuarderCtrl();
	} else {
		QLogE("Enter guard ctrl error. input password: %s", getInputPassword().toUtf8().constData());
	}
}
