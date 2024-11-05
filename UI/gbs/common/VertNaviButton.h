#ifndef VERTNAVIBUTTON_H
#define VERTNAVIBUTTON_H
#include <QAbstractButton>
#include <QIcon>
#include <QPainter>
#include <QStyleOptionButton>
#include <QPushButton>
#include <QEvent>
#include <QIcon>

/*
悬停状态的图标：在 QPushButton:hover 中通过 qproperty-icon 设置为白色版本的图标。
    "   qproperty-icon: url(:/Resources/hover-icon-white.png);"  // 悬停状态的白色图标
按下状态的图标：在 QPushButton:pressed 中也通过 qproperty-icon 设置为按下状态的白色图标。
    "   qproperty-icon: url(:/Resources/pressed-icon-white.png);"  // 按下状态的白色图标
*/

class VertNaviButton : public QPushButton {
	Q_OBJECT

public:
	explicit VertNaviButton(const QString &text, const QString &iconPath, QWidget *parent = nullptr)
		: QPushButton(parent),
		  icon(iconPath)
	{
		//          QString myStyle = R"(
		//                  QPushButton {
		//                      border: 2px solid #1B2846;
		//                      border-radius: 10px;
		//                      background-color: qlineargradient(
		//                          spread:pad,
		//                          x1:0, y1:0, x2:1, y2:1,
		//                          stop:0.1054 #1B2846,
		//                          stop:0.9062 #000000
		//                      );
		//                      color: white; /* 设置文本颜色 */
		//                      padding: 10px; /* 内边距 */
		//                  }

		//    QPushButton:hover {
		// background-color: qlineargradient(
		//     spread:pad,
		//     x1:0, y1:0, x2:1, y2:1,
		//     stop:0.1054 #2C3956,
		//     stop:0.9062 #111111
		// );
		// background-color: #1B2846;

		//    }

		//                  QPushButton:pressed {
		//                     background-color: #F9F9F9; // 按下状态背景颜色
		//                     padding-left: 1px;  /* 向左移动 3px */"
		//                     padding-top: 1px;    /* 向上移动 3px */"
		//                     color: white;  // 按下状态字体颜色
		//                  }
		//              )";
		//          setStyleSheet(myStyle);

		setLayoutDirection(Qt::LeftToRight);
		setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
		setIconSize(QSize(22, 22)); // 设置图标大小
		setText(text);
		setStyleSheet("QPushButton {"
			      "   background-color: #F9F9F9;"
			      "   border: #000000;"    // 无边框
			      "   border-radius: 5px;" // 圆角
			      "   font-size: 14px;"
			      "   padding-left: 70px;" // 调整文本位置
			      "   text-align: left;"   // 左对齐文本
			      "}"
			      "QPushButton:hover {"
			      "   background-color: #1B2846;"
			      "   color: white;" // 悬停状态字体颜色

			      "}"
			      "QPushButton:pressed {"
			      "   background-color: #F9F9F9;" // 按下状态背景颜色
			      "   padding-left: 1px;  /* 向左移动 3px */"
			      "   padding-top: 1px;    /* 向上移动 3px */"
			      "   color: white;" // 按下状态字体颜色
			      "}");
		setMouseTracking(true);
	}

protected:
	void paintEvent(QPaintEvent *event) override
	{
		// 调用父类的 paintEvent 来处理默认的绘制
		QPushButton::paintEvent(event);

		// 创建绘制器
		QPainter painter(this);

		// 在左边距离40px的位置绘制icon
		int iconX = 40;                                   // 左边距离
		int iconY = (height() - iconSize().height()) / 2; // 垂直居中
		QPixmap pixmap = icon.pixmap(iconSize());
		painter.drawPixmap(iconX, iconY, pixmap);

		// 仅在按钮悬停或选中时显示白色矩形
		if (underMouse() || isDown()) {
			painter.setBrush(QBrush(Qt::white));
			painter.setPen(Qt::NoPen); // 无边框

			// 绘制左侧的矩形，宽度为 20px，高度为按钮的 3/4，高度居中
			int rectHeight = height() * 3 / 4;
			int rectY = (height() - rectHeight) / 2;
			painter.drawRect(0, rectY, 5, rectHeight);
		}
	}

	// 按钮点击状态的变化触发重绘
	void mousePressEvent(QMouseEvent *event) override
	{
		QPushButton::mousePressEvent(event);
		update();
	}

	void mouseReleaseEvent(QMouseEvent *event) override
	{
		QPushButton::mouseReleaseEvent(event);
		update();
	}

private:
	QIcon icon;
};

#endif
