#ifndef ELLIPTICALSLIDEREXT_H
#define ELLIPTICALSLIDEREXT_H
#include <QSlider>
#include <QPainter>
#include <QDebug>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>

class EllipticalSliderExt : public QSlider {
    Q_OBJECT
public:
    explicit EllipticalSliderExt(QWidget *parent = nullptr) : EllipticalSliderExt(300, 20, parent) {

    }
    explicit EllipticalSliderExt(int w, int h, QWidget *parent = nullptr) : QSlider(Qt::Horizontal, parent) {
        /*setFixedSize(w, h);
        setRange(0, 100);*/
	    setFixedSize(w, 40);
	    setRange(0, 100);
	    //connect(this, &QSlider::valueChanged, this, [this]() { update(); }); // 连接值变化信号

        // 设置信号与槽以更新滑块
        //connect(this, &QSlider::valueChanged, this, &EllipticalSliderExt::update);
        // QHBoxLayout *layout = new QHBoxLayout(this);
        // leftLabel = new QLabel("0%", this);
        // leftLabel->setFixedSize(26, 25);
        // rightLabel =new QLabel("100%", this);
        // rightLabel->setFixedSize(45, 25);
        // layout->addWidget(leftLabel, 0, Qt::AlignLeft);
        // layout->addWidget(rightLabel, 0, Qt::AlignRight);
        // setLayout(layout);

    }

	//void setValue(int value) {
	//	QSlider::setValue(value); // 调用父类的 setValue
	//	emit valueChanged(value); // 手动触发信号
	//}
	
    void setRealSize(int w, int h) {
        setFixedSize(w, h);
    }
    void setRealRange(int begin, int end) { setRange(begin, end);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
	    Q_UNUSED(event);

	    QPainter painter(this);
	    painter.setRenderHint(QPainter::Antialiasing);

	    // 计算槽道矩形
	    QRect grooveRect = QRect(0, height() / 2 - 8, width(), 16); // 固定滑道高度为16

	    // 计算划过的部分宽度
	    int filledWidth = static_cast<int>((double(this->value()) / this->maximum()) * grooveRect.width());

	    // 绘制未划过的槽道（灰色圆角矩形）
	    painter.setBrush(QColor("#808080")); // 灰色
	    painter.setPen(Qt::NoPen);
	    painter.drawRoundedRect(grooveRect, 8, 8); // 圆角半径为8

	    // 绘制已划过的槽道（红色圆角矩形）
	    if (filledWidth > 0) {                       // 确保最左侧时无红色部分
		    painter.setBrush(QColor("#EB3F5E")); // 红色
		    painter.drawRoundedRect(QRect(grooveRect.x(), grooveRect.y(), filledWidth, grooveRect.height()), 8,
					    8);
	    }

	    // 计算原有滑块的半径和位置（手柄）
	    int handleRadius = 8; // 固定原有滑块半径为8
	    int handleX =
		    qMax(handleRadius, qMin(filledWidth, grooveRect.width() - handleRadius)); // 确保滑块不超出边界

	    // 计算原有滑块的矩形区域
	    QRect handleRect =
		    QRect(handleX - handleRadius, height() / 2 - handleRadius, handleRadius * 2, handleRadius * 2);

	    // 绘制原有滑块（红色圆点，作为手柄）
	    painter.setBrush(QColor("#EB3F5E")); // 红色
	    painter.drawEllipse(handleRect);     // 绘制原有滑块

	    // 计算右侧额外滑块的半径和位置
	    int extraHandleRadius = height() / 2; // 新滑块半径，超过滑道高度（16）
	    int extraHandleX = filledWidth;       // 新滑块位于红色填充部分的右侧边缘

	    // 确保新滑块不超出滑槽的边界
	    extraHandleX = qMax(extraHandleRadius, qMin(extraHandleX, grooveRect.width() - extraHandleRadius));

	    // 计算右侧额外滑块的矩形区域
	    QRect extraHandleRect = QRect(extraHandleX - extraHandleRadius, height() / 2 - extraHandleRadius,
					  extraHandleRadius * 2, extraHandleRadius * 2);

	    // 绘制右侧额外滑块（红色圆点，与划过部分颜色一致）
	    painter.setBrush(QColor("#EB3F5E"));  // 红色
	    painter.drawEllipse(extraHandleRect); // 绘制新滑块

	    // 绘制标签文本（仅显示在原有滑块附近）
	    painter.setPen(Qt::white); // 设置文本颜色为白色，与灰色背景对比
	    QFont font = painter.font();
	    font.setPointSize(10); // 设置字体大小为10点
	    painter.setFont(font);
	    painter.drawText(handleX + handleRadius - 20, grooveRect.y() - 20, 40, 15, Qt::AlignCenter,
			     QString::number(this->value()) + "%");

	    // 调试：绘制矩形检查文本位置
	    // painter.setPen(Qt::red);
	    // painter.drawRect(handleX + handleRadius - 20, grooveRect.y() - 20, 40, 15);
    }
    private:
    QLabel* leftLabel;
    QLabel* rightLabel;
};


#endif //ELLIPTICALSLIDEREXT_H
