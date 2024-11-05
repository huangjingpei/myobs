#ifndef TESTDANMAKUWIDGET_H
#define TESTDANMAKUWIDGET_H

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
#include "../common/DanmakuWidget.h"

class TestDanmakuWidget : public QWidget
{
	Q_OBJECT

public:
	TestDanmakuWidget(QWidget *parent = nullptr) : QWidget(parent)
	{
		// 初始化布局
		mainLayout = new QVBoxLayout();
		mainLayout->setSpacing(6);  // 设置widget之间的固定间距为6像素
		mainLayout->setAlignment(Qt::AlignBottom);  // 保证最新widget在最底部

		QWidget *containerWidget = new QWidget();  // 用于容纳所有widgets
		containerWidget->setLayout(mainLayout);

		// ScrollArea 包裹 widgets 的容器
		scrollArea = new QScrollArea(this);
		scrollArea->setWidget(containerWidget);
		scrollArea->setWidgetResizable(true);
		scrollArea->setStyleSheet(
			"QScrollArea { border: none; }"
			"QScrollBar:vertical { width: 10px; }"  // 调整垂直滚动条的宽度
			"QScrollBar::handle:vertical { background: #CCCCCC; }"  // 滚动条手柄颜色
			);

		// 外部主布局
		QVBoxLayout *outerLayout = new QVBoxLayout(this);
		outerLayout->addWidget(scrollArea);

		// 添加按钮来添加新 widget
		QPushButton *addButton = new QPushButton("Add New Widget", this);
		connect(addButton, &QPushButton::clicked, this, &TestDanmakuWidget::addNewWidget);
		outerLayout->addWidget(addButton);
	}

public slots:
	// 添加新 widget 的槽函数
	void addNewWidget() {
		++danmaindex;
		QString text = "这是动态设置的第" + QString::number(danmaindex) + "行文本";
		addNewWidget2(text, ":Resources/gbs-logo-douyin.png", "文本会自动换行以适应窗口宽度。我这里就需要看到你换行,这是动态设置的第二行文本。文本会自动换行以适应窗口宽度。我这里就需要看到你换行");

	}
protected:
	void addNewWidget2(const QString &text, const QString &imagePath, const QString &text2)
	{
		// 创建新 widget
		DanmakuWidget *newWidget = new DanmakuWidget(this);
		newWidget->setFirstRowContent(text, imagePath);
		newWidget->setSecondRowContent(text2);

		// 添加到布局
		widgetList.append(newWidget);

		// 插入到布局的最底部
		mainLayout->insertWidget(mainLayout->count(), newWidget);  // 最新的 widget 添加到布局的最后
		qDebug() << "mainLayout count " << mainLayout->count();

		// 滚动到最底部显示最新添加的widget
		QScrollBar *vScrollBar = scrollArea->verticalScrollBar();
		vScrollBar->setValue(vScrollBar->maximum());

		// 检查是否超过 50 个 widget
		if (widgetList.size() > 50) {
			// 移除最早的 widget
			DanmakuWidget *oldestWidget = widgetList.takeFirst();
			mainLayout->removeWidget(oldestWidget);
			delete oldestWidget;
		}
	}

private:
	QScrollArea *scrollArea;
	QVBoxLayout *mainLayout;
	QList<DanmakuWidget*> widgetList;
	int danmaindex;
};

#endif //TESTDANMAKUWIDGET_H
