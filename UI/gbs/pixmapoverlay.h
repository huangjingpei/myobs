#ifndef PIXMAPOVERLAY_H
#define PIXMAPOVERLAY_H

#include <QWidget>
#include <QPainter>
#include <QPixmap>

class PixmapOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit PixmapOverlay(QWidget *parent = nullptr);
    PixmapOverlay(QPixmap &pixmap1, QPixmap &pixmap2, QWidget *parent = nullptr);
public:
    QPixmap* overlay();
protected:
    void paintEvent(QPaintEvent *) override;

signals:

private:
    QPixmap pixmap1;
    QPixmap pixmap2;
    QPainter painter;
};

#endif // PIXMAPOVERLAY_H
