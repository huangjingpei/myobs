#include "gbs-login.h"
#include "./ui_gbslogin.h"
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include "pixmapoverlay.h"

GBSLogin::GBSLogin(QWidget *parent) : QWidget(parent), ui(new Ui::GBSLogin)
{
	ui->setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);



	QPushButton *minimizeButton = new QPushButton("-", this);
	QPushButton *maximizeButton = new QPushButton("+", this);
	QPushButton *closeButton = new QPushButton("x", this);
	// 设置按钮的大小
	minimizeButton->setFixedSize(40, 40);
	maximizeButton->setFixedSize(40, 40);
	closeButton->setFixedSize(40, 40);

	// 设置样式表
	minimizeButton->setStyleSheet("background-color: #CCCCCC; border: none;");
	maximizeButton->setStyleSheet("background-color: #CCCCCC; border: none;");
	closeButton->setStyleSheet("background-color: #FF3333; color: white; border: none;");

	// 布局管理：把按钮放在右上角
	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addStretch(); // 推动按钮靠右
	buttonLayout->addWidget(minimizeButton);
	buttonLayout->addWidget(maximizeButton);
	buttonLayout->addWidget(closeButton);

	QVBoxLayout *rightLayout = new QVBoxLayout;
	rightLayout->addLayout(buttonLayout);

	// 连接信号槽
	connect(minimizeButton, &QPushButton::clicked, this, &GBSLogin::showMinimized);
	connect(maximizeButton, &QPushButton::clicked, this, &GBSLogin::showFullScreen);
	connect(closeButton, &QPushButton::clicked, this, &GBSLogin::close);

}

GBSLogin::~GBSLogin()
{
	delete ui;
}
