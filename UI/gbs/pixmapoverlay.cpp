#include "pixmapoverlay.h"

PixmapOverlay::PixmapOverlay(QWidget *parent)
    : QWidget{parent}
{}

PixmapOverlay::PixmapOverlay(QPixmap &pixmap1, QPixmap &pixmap2, const QRect &rect, QWidget *parent)
	: QWidget(parent),
	  pixmap1(pixmap1),
	  pixmap2(pixmap2),
	  rect(rect)
{
	qDebug() << "pixmap1 size w" << pixmap1.width() << " h " << pixmap1.height();
	qDebug() << "pixmap2 size w" << pixmap2.width() << " h " << pixmap2.height();
    //resize(pixmap1.size());
}

void PixmapOverlay::setOverlayRect(const QRect &rect) {
	this->rect = rect;
    update();  // 重绘
}

void PixmapOverlay::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.drawPixmap(0, 0, 739, 990, pixmap1);  // 先绘制背景图

    // 在指定位置叠加第二张图片
    painter.drawPixmap(rect, pixmap2);
}

QPixmap* PixmapOverlay::overlay() {
    QPixmap *result = new QPixmap(pixmap1.width(), pixmap1.height());
    result->fill(Qt::transparent);
    {
	// 启用抗锯齿和文本抗锯齿
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::TextAntialiasing, true);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        QPainter painter(result);
	painter.drawPixmap(0, 0, 739, 990, pixmap1); // 背景图片
        painter.drawPixmap(rect, pixmap2);  // 使用指定位置叠加
    }
    return result;
}
