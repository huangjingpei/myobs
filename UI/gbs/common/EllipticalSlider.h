
#include <QSlider>
#include <QPainter>

class EllipticalSlider : public QSlider {
        Q_OBJECT

public:
    explicit EllipticalSlider(int w, int h, QWidget *parent = nullptr) : QSlider(Qt::Horizontal, parent) {
        setFixedSize(w, h);
        setRange(0, 1);

    }

    explicit EllipticalSlider(QWidget *parent = nullptr) : QSlider(Qt::Horizontal, parent) { 

    }


protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // 计算槽道矩形
        QRect grooveRect = QRect(0, 0, width(), height());

        // 绘制椭圆形槽道
        if (this->value() == 1) {
            painter.setBrush(QColor("#00C566"));
        } else {
            painter.setBrush(QColor("#000000"));

        }
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(grooveRect, 10, 10);

        // 计算滑块的位置
        int handleX = static_cast<int>((double(this->value()) / this->maximum()) * (width() - 24));
        //qDebug() << "handle x " << handleX << " y " << height() / 2 - 10;
        QRect handleRect = QRect(handleX, (height()-18)/2, 24, 18);

        // 绘制椭圆形滑块
        painter.setBrush(Qt::white);
        if (this->value() == 1) {
            painter.setPen(QColor("#00C566"));
        } else {
            painter.setPen(QColor("#000000"));
        }
        painter.drawRoundedRect(handleRect,10, 10);
    }

};