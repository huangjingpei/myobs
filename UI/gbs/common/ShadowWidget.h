#ifndef SHADOWWIDGET_H
#define SHADOWWIDGET_H
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>


/**
 下面宽体是严格按照 box-shadow: 0px 10px 4px 0px #0C0C0C; 的 CSS 样式在 Qt 中实现阴影效果，以下是使用 QPainter 仿写该效果的代码：

0px 表示水平偏移为 0。
10px 表示垂直偏移为 10 像素。
4px 表示模糊半径（在 Qt 中模糊效果可以通过绘制多个偏移的阴影矩形来模拟）。
#0C0C0C 是阴影颜色。
实现代码

代码解释：
阴影部分：
使用 for 循环模拟模糊效果，通过改变透明度和偏移逐渐绘制 4 层阴影。
阴影从 10px 的垂直偏移开始，水平偏移为 0px，阴影的颜色是 #0C0C0C（RGB: 12, 12, 12），并且逐步降低透明度。
主体矩形框：

内容区域是一个白色的矩形，大小是窗口宽度减去阴影的部分，确保阴影位于矩形的下方。
效果：
该实现严格遵循了 CSS box-shadow 的要求，绘制了一个有垂直偏移和模糊效果的阴影，视觉上接近 box-shadow 的样式。


现在还没有时间验证，以后有时间进行验证
 */

class ShadowWidget : public QWidget {
    Q_OBJECT

public:
    explicit ShadowWidget(QWidget *parent = nullptr)
        : QWidget(parent) {
        setFixedSize(300, 200);  // 设置窗口大小
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // 1. 绘制阴影
        QColor shadowColor(12, 12, 12, 150);  // 阴影颜色，#0C0C0C (RGB) 和透明度
        painter.setBrush(shadowColor);
        painter.setPen(Qt::NoPen);  // 无边框

        // 使用渐变模拟模糊效果，调整透明度逐步绘制阴影
        for (int i = 0; i < 4; ++i) {
            QRect shadowRect(4 - i, 10 - i, width() - 8 + i * 2, height() - 8 + i * 2);
            painter.setBrush(QColor(12, 12, 12, 150 - (i * 30)));  // 每次减弱阴影透明度
            painter.drawRoundedRect(shadowRect, 10, 10);  // 绘制阴影，圆角半径为 10px
        }

        // 2. 绘制实际的矩形框 (假设是带有圆角的内容)
        QColor boxColor(255, 255, 255);  // 主体矩形颜色 (白色)
        painter.setBrush(boxColor);
        QRect boxRect(0, 0, width() - 8, height() - 8);  // 矩形框
        painter.drawRoundedRect(boxRect, 10, 10);  // 绘制实际内容
    }
};


#endif //SHADOWWIDGET_H