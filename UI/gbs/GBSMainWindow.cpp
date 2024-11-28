#include "gbsmainwindow.h"
#include "ui_gbsmainwindow.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

#include "pixmapoverlay.h"

GBSMainWindow::GBSMainWindow(QWidget *parent)
	: QMainWindow(parent),
	  ui(new Ui::GBSMainWindow),
	  gbsMainForm(new GBSMainForm)
{
	ui->setupUi(this);
	gbsMainForm->setMainWindow(this);
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);

	QLabel *imageLabel = new QLabel(this);
	QPixmap pixmapBG(":gbs/images/gbs/login/login-right-bg.png");
	QPixmap pixmapBG2(":gbs/images/gbs/login/login-right-middle-bg.png");

	PixmapOverlay *overlay = new PixmapOverlay(pixmapBG, pixmapBG2);
	QPixmap *pixmap = overlay->overlay();

	imageLabel->setPixmap(pixmap->scaled(600, 832, Qt::KeepAspectRatio));

	ui->horizontalLayout->addWidget(imageLabel);


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
	rightLayout->addWidget(gbsMainForm);

	// 连接信号槽
	connect(minimizeButton, &QPushButton::clicked, this, &GBSMainWindow::showMinimized);
	connect(maximizeButton, &QPushButton::clicked, this, &GBSMainWindow::showFullScreen);
	connect(closeButton, &QPushButton::clicked, this, &GBSMainWindow::close);


	ui->horizontalLayout->addLayout(rightLayout);

}

GBSMainWindow::~GBSMainWindow()
{
	delete ui;
}

void GBSMainWindow::slotDestroyLoginWindow() {
	close();
}


