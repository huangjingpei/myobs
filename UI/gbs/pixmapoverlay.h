#ifndef PIXMAPOVERLAY_H
#define PIXMAPOVERLAY_H

#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QRect>

class PixmapOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit PixmapOverlay(QWidget *parent = nullptr);
	explicit PixmapOverlay(QPixmap &pixmap1, QPixmap &pixmap2, const QRect &rect = QRect(0, 0, 0, 0),
			       QWidget *parent = nullptr);

public:
	QPixmap *overlay();
    void setOverlayRect(const QRect &rect); // 新增函数设置叠加位置

protected:
    void paintEvent(QPaintEvent *) override;

signals:

private:
    QPixmap pixmap1;
    QPixmap pixmap2;
    QPainter painter;
    QRect rect; // 叠加位置

};

#endif // PIXMAPOVERLAY_H
