#include "gbsbasicwindow.h"
#include "ui_gbsbasicwindow.h"
#include <QMenuBar>
#include <QApplication>
#include <QAction>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>

GBSBasicWindow::GBSBasicWindow(QWidget *parent)
	: QMainWindow(parent),
	  ui(new Ui::GBSBasicWindow),
	  danmaWidget(new DanmakuWidget)
{
	ui->setupUi(this);


	QHBoxLayout *mainLayout = new QHBoxLayout(ui->centralwidget);

	QMenuBar* menuBar = ui->menubar;
	QMenu *liveBroadcast = menuBar->addMenu("直播端");
	QMenu *parellalDlivery = menuBar->addMenu("矩阵分发");
	QMenu *intimeTranslate = menuBar->addMenu("实时翻译");
	QMenu *gbsshop = menuBar->addMenu("供应链");
	QMenu *aiModel = menuBar->addMenu("大模型");
	QMenu *dataShowPannel= menuBar->addMenu("数据大屏");
	QMenu *danmuScratch = menuBar->addMenu("弹幕获取");


	QAction* exitAction = liveBroadcast->addAction("退出(&X)");

	ui->naviFrame->setGeometry(1, 90, 90, 710);

	mainLayout->addWidget(ui->naviFrame);

	mainLayout->setStretch(0, 1);  // 第一个子布局占 1 份


	QVBoxLayout *danmaLayout = new QVBoxLayout;

	danmaLayout->addWidget(ui->danmaFrame);


	danmaLayout->addWidget(danmaWidget);

	mainLayout->addLayout(danmaLayout);

	mainLayout->setStretch(1, 8);  // 第一个子布局占 1 份


	mainLayout->addWidget(ui->liveFrame);

	mainLayout->setStretch(2, 8);  // 第一个子布局占 1 份


}

GBSBasicWindow::~GBSBasicWindow()
{

	delete ui;
}


