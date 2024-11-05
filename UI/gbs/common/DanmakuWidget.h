#ifndef GBSDANMAKUWIDGET_H
#define GBSDANMAKUWIDGET_H
#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>
#include <QSizePolicy>

class DanmakuWidget : public QWidget
{
	Q_OBJECT

public:
	// 构造函数
	DanmakuWidget(QWidget *parent = nullptr) : QWidget(parent)
	{
		// 创建第一行的文本标签和图片标签
		textLabel1 = new QLabel();
		textLabel1->setAlignment(Qt::AlignLeft);  // 文本左对齐

		imageLabel = new QLabel();
		imageLabel->setAlignment(Qt::AlignRight);  // 图片右对齐
		imageLabel->setFixedSize(22, 22);  // 固定图片大小为 22x22
		imageLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		// 第一行的水平布局
		QHBoxLayout *firstRowLayout = new QHBoxLayout;
		firstRowLayout->addWidget(textLabel1);  // 添加文本标签
		firstRowLayout->addWidget(imageLabel);  // 添加图片标签
		firstRowLayout->setSpacing(10);  // 设置第一行内的组件之间的间距
		firstRowLayout->setContentsMargins(38, 0, 0, 0);  // 减少边距

		// 创建第二行的文本标签（支持换行）
		textLabel2 = new QLabel();
		textLabel2->setWordWrap(true);  // 启用换行
		textLabel2->setAlignment(Qt::AlignLeft);  // 文本左对齐

		// 第二行的布局
		QVBoxLayout *secondRowLayout = new QVBoxLayout;
		secondRowLayout->addWidget(textLabel2);  // 添加第二行文本标签
		secondRowLayout->setSpacing(0);  // 第二行内的间距

		// 主布局
		QVBoxLayout *mainLayout = new QVBoxLayout(this);
		mainLayout->addLayout(firstRowLayout);  // 添加第一行布局
		mainLayout->addLayout(secondRowLayout);  // 添加第二行布局
		mainLayout->setSpacing(5);  // 设置第一行和第二行之间的间距为 5 像素
		mainLayout->setContentsMargins(0, 0, 0, 0);  // 减少整体边距

		// 设置窗口大小策略为自适应
		setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	}

	// 设置第一行的文本和图片
	void setFirstRowContent(const QString &text, const QString &imagePath)
	{
		textLabel1->setText(text);  // 设置文本
		QPixmap pixmap(imagePath);  // 加载图片
		imageLabel->setPixmap(pixmap.scaled(22, 22, Qt::KeepAspectRatio));  // 设置图片大小为 22x22
	}

	// 设置第二行的文本
	void setSecondRowContent(const QString &text)
	{
		textLabel2->setText(text);  // 设置文本
	}

private:
	QLabel *textLabel1;  // 第一行的文本标签
	QLabel *imageLabel;  // 第一行的图片标签
	QLabel *textLabel2;  // 第二行的文本标签
};

#endif //GBSDANMAKUWIDGET_H

// int main(int argc, char *argv[])
// {
//     QApplication app(argc, argv);

//     // 创建主窗口
//     DanmakuWidget widget;
//     widget.setWindowTitle("Qt Widget Example");
//     widget.resize(400, 200);

//     // 设置第一行的文本和图片
//     widget.setFirstRowContent("这是动态设置的第一行文本", ":Resources/gbs-logo-douyin.png");

//     // 设置第二行的文本
//     widget.setSecondRowContent("这是动态设置的第二行文本。文本会自动换行以适应窗口宽度。我这里就需要看到你换行");

//     // 显示窗口
//     widget.show();

//     return app.exec();
// }
