#ifndef __DOUBLE_TEXT_BUTTON_H__
#define __DOUBLE_TEXT_BUTTON_H__
#include <QAbstractButton>
#include <QIcon>
#include <QPainter>
#include <QStyleOptionButton>
#include <QPushButton>
#include <QIcon>
#include <QPushButton>
#include <QPainter>
#include <QPixmap>
#include <QVBoxLayout>

class DoubleTextButton : public QPushButton {
    Q_OBJECT
public:
    explicit DoubleTextButton(QWidget *parent = nullptr) : QPushButton(parent) {
        setFixedSize(150, 137); // 按钮大小
        setStyleSheet("border: none;"); // 移除默认边框
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);

        // 设置抗锯齿
        painter.setRenderHint(QPainter::Antialiasing);

        // 设置背景渐变色
        QLinearGradient gradient(rect().topLeft(), rect().bottomRight());
        if (!wasPressed) {
            gradient.setColorAt(0.1054, QColor("#1B2846"));
            gradient.setColorAt(0.9062, QColor("#000000"));
        } else {
            gradient.setColorAt(0.1054, QColor("#65ffd1"));
            gradient.setColorAt(0.9062, QColor("#00C566"));
        }
        painter.setBrush(gradient);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(rect(), 16, 16); // 圆角矩形背景，16px 圆角

        // 设置文本颜色
        painter.setPen(QColor("#FFFFFF"));
        QFont font = painter.font();


        font.setPixelSize(40); // 字体大小
        font.setBold(true);
        painter.setFont(font);

        QRect firstLineRect = rect();
        int firstLineY = (rect().height() - 40 - 10) / 2; // 确保第一行文本垂直居中，减去第二行的高度和间距
        firstLineRect.setTop(firstLineY); // 设置顶部位置
        firstLineRect.setHeight(56); // 第一行文本的高度
        painter.drawText(firstLineRect, Qt::AlignHCenter | Qt::AlignTop, firstLineText);


        font.setPixelSize(10); // 字体大小
        font.setBold(false);
        painter.setFont(font);

        QRect secondLineRect = rect();
        secondLineRect.setTop(firstLineY + 40 + 20); // 紧贴第一行正下方，间距10像素
        secondLineRect.setHeight(14); // 第二行文本的高度
        painter.drawText(secondLineRect, Qt::AlignHCenter | Qt::AlignTop, secondLineText);

        QPushButton::paintEvent(event); // 保留按钮的默认绘制事件
    }

public:

    void pressed(QString firstText, QString secondText, bool pressed) {
        wasPressed = pressed;
        firstLineText = firstText;
        secondLineText = secondText;
        update();
    }

private:
    bool wasPressed = false;
    QString firstLineText = "开播";
    QString secondLineText = "已关播";
};
#endif //__DOUBLE_TEXT_BUTTON_H__
