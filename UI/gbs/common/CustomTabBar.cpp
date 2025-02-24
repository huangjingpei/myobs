#include "CustomTabBar.h"
void CustomTabBar::paintEvent(QPaintEvent *event) {
    QTabBar::paintEvent(event); // 调用父类绘制默认标签

    QPainter painter(this);
    int currentIndex = QTabBar::currentIndex(); // 获取当前选中的标签索引

    for (int i = 0; i < count(); ++i) {
        if (i == currentIndex) {
            QStyleOptionTab tabOption;
            initStyleOption(&tabOption, i);
            QRect textRect = style()->subElementRect(QStyle::SE_TabBarTabText, &tabOption, this);

            // 计算短线条位置
            int textBottom = textRect.bottom();
            int lineY = textBottom + 20; // 距离文本底部 20 像素
            int lineX = textRect.x() + textRect.width() / 2 - 12; // 居中，24/2=12
            int lineWidth = 24; // 短线条长度
            int lineHeight = 4; // 短线条高度

            // 获取标签矩形，确保短线条在范围内
            QRect tabRect = QTabBar::tabRect(i);
            lineX = qMax(tabRect.x(), qMin(lineX, tabRect.x() + tabRect.width() - lineWidth));
            lineY = qMax(tabRect.y(), qMin(lineY, tabRect.y() + tabRect.height() - lineHeight));

            // 绘制短线条
            painter.setPen(Qt::NoPen); // 不使用笔刷边框
            painter.setBrush(QBrush(QColor("#00C566"))); // 设置颜色为 #00C566
            painter.drawRect(lineX, lineY, lineWidth, lineHeight); // 绘制矩形短线条
        }
    }
}
