#include "pixmapoverlay.h"

PixmapOverlay::PixmapOverlay(QWidget *parent)
    : QWidget{parent}
{}


PixmapOverlay::PixmapOverlay(QPixmap &pixmap1, QPixmap &pixmap2, QWidget *parent)
    : QWidget(parent), pixmap1(pixmap1), pixmap2(pixmap2) {

    resize(pixmap1.size());

}

void PixmapOverlay::paintEvent(QPaintEvent *event) {

    painter.drawPixmap(0, 0, pixmap1);
    painter.drawPixmap((pixmap1.width() - pixmap2.width())/2, (pixmap1.height() - pixmap2.height())/2, pixmap2);
}

QPixmap* PixmapOverlay::overlay() {
    QPixmap *result = new QPixmap(pixmap1.width(), pixmap1.height());
    result->fill(Qt::transparent);
    {
        QPainter painter(result);
        painter.drawPixmap(0, 0, pixmap1);
        painter.drawPixmap((pixmap1.width() - pixmap2.width())/2, (pixmap1.height() - pixmap2.height())/2, pixmap2);
    }
    return result;
}

