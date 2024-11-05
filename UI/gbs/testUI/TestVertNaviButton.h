#ifndef TestVertNaviButton_H
#define TestVertNaviButton_H

// #include <QWidget>

// namespace Ui {
// class TestDanmakuWidget;
// }

// class TestDanmakuWidget : public QWidget {
// 	Q_OBJECT

// public:
// 	explicit TestDanmakuWidget(QWidget *parent = nullptr);
// 	~TestDanmakuWidget();

// private:
// 	Ui::TestDanmakuWidget *ui;
// };

// #endif // TESTDANMAKUWIDGET_H


#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QList>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>

#include "../common/VertNaviButton.h"
#include "../common/FatButton.h"
#include "../common/VertNaviButtonExt.h"

class TestVertNaviButton : public QWidget
{
	Q_OBJECT

public:
	TestVertNaviButton(QWidget *parent = nullptr) : QWidget(parent)
	{
		// 初始化布局
		mainLayout = new QVBoxLayout();
		setLayout(mainLayout);
		VertNaviButton* button1 = new VertNaviButton("设备信息", ":Resources/gbs-logo.png",  this);
		VertNaviButton* button2 = new VertNaviButton("设备地址", ":Resources/gbs-logo-tiktok.png",  this);
		mainLayout->addWidget(button1);
		mainLayout->setSpacing(10);  // 设置widget之间的固定间距为6像素
		mainLayout->addWidget(button2);

		FatButton *button3 = new FatButton("我的测试", ":Resources/gbs-logo-shopee.png", this);

		button3->setFixedSize(100, 40);
		FatButton *button4 = new FatButton("我的测试", ":Resources/gbs-logo-pddlive.png", this);
		mainLayout->addWidget(button3);
		mainLayout->addWidget(button4);
		button4->setFixedSize(100, 40);

		VertNaviButtonExt* button5 = new VertNaviButtonExt("设备信息", ":Resources/gbs-logo.png",  this);
		VertNaviButtonExt* button6 = new VertNaviButtonExt("设备da dd 地址", ":Resources/gbs-logo-tiktok.png",  this);
		mainLayout->addWidget(button5);
		mainLayout->setSpacing(10);  // 设置widget之间的固定间距为6像素
		mainLayout->addWidget(button6);
		button5->setFixedSize(180, 30);

		button6->setFixedSize(180, 30);


		mainLayout->setAlignment(Qt::AlignBottom);  // 保证最新widget在最底部


	}


protected:


private:
	QVBoxLayout *mainLayout;
};

#endif //TestVertNaviButton_H
