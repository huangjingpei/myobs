#include "GBSBizLiveGuarderAuth.h"
#include "ui_GBSBizLiveGuarderAuth.h"
#include "GBSBizLiveGuarder.h"
#include <QList>
#include<QLineEdit>
#define AUTH_CODE_NUM 5
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

    connect(ui->pushButton, &QPushButton::clicked, this, [this](){
        if (inputPassword == presetPassword) {
            emit enterGuarderCtrl();
        }
    });

}

GBSBizLiveGuarderAuth::~GBSBizLiveGuarderAuth()
{
    qlists.clear();
	delete ui;
}

void GBSBizLiveGuarderAuth::focusNextInput(int currentIndex) {
    if ((currentIndex >=0) && (currentIndex < 5)) {
        inputPassword+=qlists[currentIndex]->text();
        qDebug() << "inputPassword " << inputPassword;
    }
    if (currentIndex < 4) {
        qlists[currentIndex + 1]->setFocus(); // 跳到下一个输入框
    }
}

void GBSBizLiveGuarderAuth::keyPressEvent(QKeyEvent *event) {
    // if (event->key() == Qt::Key_Tab) {
    //     QWidget *focusedWidget = focusWidget();
    //     for (int i = 0; i < 5; ++i) {
    //         if (qlists[i] == focusedWidget && i < 4) {
    //             qlists[i + 1]->setFocus();
    //             return;
    //         }
    //     }
    // }
    QWidget::keyPressEvent(event); // 调用父类的默认行为
}


QString GBSBizLiveGuarderAuth::getPresetPassword() {
    return presetPassword;
}
QString GBSBizLiveGuarderAuth::getInputPassword() {
    return inputPassword;
}
void GBSBizLiveGuarderAuth::setPresetPassword(QString password) {
    presetPassword = password;
}
