#include "GBSMainProfile.h"
#include "ui_GBSMainProfile.h"
#include "common/VertNaviButton.h"
#include <QMouseEvent>
#include <QEvent>
#include <QSpacerItem>


GBSMainProfile::GBSMainProfile(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GBSMainProfile)
{

    setSizePolicy(
        QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint |
                   Qt::WindowMaximizeButtonHint |
                   Qt::WindowCloseButtonHint);
    ui->setupUi(this);

    VertNaviButton* btnAccount = new VertNaviButton("账号资料", ":gbs/images/gbs/biz/gbs-menu-profile.png", this);
    VertNaviButton* btnAboutME = new VertNaviButton("关于我们", ":gbs/images/gbs/biz/gbs-menu-about-me.png", this);
    VertNaviButton* btnSoftWare = new VertNaviButton("软件更新", ":gbs/images/gbs/biz/gbs-menu-software-update.png", this);
    VertNaviButton* btnSetting = new VertNaviButton("高级设置", ":gbs/images/gbs/biz/gbs-menu-setting.png", this);
    VertNaviButton* btnHotkey = new VertNaviButton("快捷键", ":gbs/images/gbs/biz/gbs-menu-hotkey.png", this);
    VertNaviButton* btnAPI = new VertNaviButton("API接口", ":gbs/images/gbs/biz/gbs-menu-api.png", this);
    VertNaviButton* btnMaterial = new VertNaviButton("素材背景库", ":gbs/images/gbs/biz/gbs-menu-material.png", this);
    VertNaviButton* btnDriver = new VertNaviButton("摄像头驱动", ":gbs/images/gbs/biz/gbs-menu-driver.png", this);
    VertNaviButton* btnSquare = new VertNaviButton("音色广场", ":gbs/images/gbs/biz/gbs-menu-timbre-square.png", this);


    btnAccount->setFixedSize(300, 40);
    btnAboutME->setFixedSize(300, 40);
    btnSoftWare->setFixedSize(300, 40);
    btnSetting->setFixedSize(300, 40);
    btnHotkey->setFixedSize(300, 40);
    btnAPI->setFixedSize(300, 40);
    btnMaterial->setFixedSize(300, 40);
    btnDriver->setFixedSize(300, 40);
    btnSquare->setFixedSize(300, 40);

    ui->verticalLayout->addWidget(btnAccount);
    ui->verticalLayout->addWidget(btnAboutME);
    ui->verticalLayout->addWidget(btnSoftWare);
    ui->verticalLayout->addWidget(btnSetting);
    ui->verticalLayout->addWidget(btnHotkey);
    ui->verticalLayout->addWidget(btnAPI);
    ui->verticalLayout->addWidget(btnMaterial);
    ui->verticalLayout->addWidget(btnDriver);
    ui->verticalLayout->addWidget(btnSquare);
    QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayout->addSpacerItem(verticalSpacer);


    ui->pushButton_2->setStyleSheet(
        "QPushButton {"
        "   background-color: #8F92A1;"
        "   border: #000000;" // 无边框
        "   border-radius: 10px;" // 圆角
        "   font-size: 14px;"
        "   text-align: center;" // 左对齐文本
        "}"

        "QPushButton:hover {"
        "   background-color: #1B2846;"
        "   color: white;"  // 悬停状态字体颜色

        "}"
        "QPushButton:pressed {"
        "   background-color: #F9F9F9;" // 按下状态背景颜色
        "   padding-left: 1px;  /* 向左移动 3px */"
        "   padding-top: 1px;    /* 向上移动 3px */"
        "   color: white;"  // 按下状态字体颜色
        "}"
        );
    connect(ui->pushButton_2,  &QPushButton::clicked, this, &GBSMainProfile::exitSystemAndGoLogin);
}

void GBSMainProfile::exitSystemAndGoLogin(bool cheked) {
    close();

}
GBSMainProfile::~GBSMainProfile()
{
    delete ui;
}


