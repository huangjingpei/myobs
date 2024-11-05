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
        setFixedSize(w, h);
        setRange(0, 100);

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


    void setRealSize(int w, int h) {
        setFixedSize(w, h);
    }
    void setRealRange(int begin, int end) { setRange(begin, end);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // 计算槽道矩形
        QRect grooveRect = QRect(0, height() / 2 - 8,
                                 width(), height()/2); //0, 2, 300, 10

        // 计算划过的部分宽度
        int filledWidth = static_cast<int>((double(this->value()) / this->maximum()) * grooveRect.width());

        // 绘制已划过的槽道（红色椭圆）
        painter.setBrush(QColor("#EB3F5E")); // 红色
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(QRect(grooveRect.x(), grooveRect.y(), filledWidth, grooveRect.height()), height()/4, height()/4);

        // 绘制未划过的槽道（灰色椭圆）
        painter.setBrush(QColor("#808080")); // 灰色
        painter.drawRoundedRect(QRect(grooveRect.x() + filledWidth, grooveRect.y(),
                                      grooveRect.width() - filledWidth, grooveRect.height()), height()/4, height()/4);

        // 计算滑块的位置
        int handleX = filledWidth - 6; // 中心对齐滑块
        QRect handleRect = QRect(handleX,height()/2 - 10, height()/2 +4, height()/2 +4); // 小圆点的矩形区域

        // 绘制滑块（小红点）
        painter.setBrush(QColor("#EB3F5E")); // 红色
        painter.drawEllipse(handleRect); // 绘制滑块

        // 绘制标签文本
        painter.setPen(Qt::black); // 设置文本颜色
        painter.drawText(handleX + 14, grooveRect.y() + 10, QString::number(this->value()) + "%");
    }

private:
    QLabel* leftLabel;
    QLabel* rightLabel;
};


#endif //ELLIPTICALSLIDEREXT_H
