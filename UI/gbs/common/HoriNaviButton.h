#ifndef HORINAVIBUTTON_H
#define HORINAVIBUTTON_H

#include <QPushButton>
#include <QPixmap>
#include <QMouseEvent>
#include <QLabel>
#include <QString>
#include <QPaintEvent>
#include <QPainter>
class HoriNaviButton : public QPushButton {
    Q_OBJECT
public:
    explicit HoriNaviButton(QWidget *parent = nullptr) : HoriNaviButton("", parent){

    }

    explicit HoriNaviButton(QString text, QWidget *parent = nullptr)
        : QPushButton("", parent), textLabel(new QLabel(this)), imageLabel(new QLabel(this)) {

        setFixedSize(90, 55); // 总高度包含按钮和图片
        // 设置按钮样式
        setCheckable(true); // 使按钮可被选中
        setText("");
        textLabel->setText(text);
        textLabel->setAlignment(Qt::AlignCenter); // 居中对齐
        textLabel->setFixedSize(90, 40); // 设置按钮的宽高
        setStyleSheet("font-size: 16px; border: none;"); // 设置无边框样式


        // 创建图片标签
        imageLabel->setPixmap(QPixmap(":gbs/images/gbs/biz/gbs-navi-indikator.png")); // 替换为您的图片路径
        imageLabel->setFixedSize(32, 15); // 设置图片大小
        imageLabel->setAlignment(Qt::AlignHCenter); // 居中对齐
        imageLabel->setVisible(false); // 初始时隐藏图片
        //qDebug() << "HoriNaviButton " << " width " << width() << " height " <<  height() << " image width "<< imageLabel->width() << " iamge height " << imageLabel->height();
        //imageLabel->move((width() - imageLabel->width()) / 2, height()); // 将图片放在按钮正下方
        imageLabel->move((width() - imageLabel->width()) / 2, 40); // 将图片放在按钮正下方
        // 连接信号和槽
        //connect(this, &QPushButton::toggled, this, &HoriNaviButton::updateStyle);
        //connect(this, &QPushButton::clicked, this, &HoriNaviButton::onButtonClicked);
    }
// protected:
//     void paintEvent(QPaintEvent *event) override {
//         QPushButton::paintEvent(event); // 调用基类的绘制方法

//         QPainter painter(this);
//         painter.setPen(Qt::red); // 设置绘制颜色为红色

//         // 绘制图片位置的矩形
//         QRect imageRect((width() - imageLabel->width()) / 2, 25, imageLabel->width(), imageLabel->height());
//         painter.drawRect(imageRect); // 绘制矩形边框

//         // 输出调试信息
//         qDebug() << "ImageLabel position:" << imageRect;
//     }
    void setRealText(QString text) {
        textLabel->setText(text);
    }
protected:
    void focusInEvent(QFocusEvent *event) override {
        // imageLabel->setVisible(true); // 根据按钮状态显示或隐藏图片

        // textLabel->setStyleSheet("background-color: #EB3F5E; font-size: 16px; border-radius: 16px;"); // 得到焦点时更新样式
        QPushButton::focusInEvent(event); // 继续处理事件
    }

    void focusOutEvent(QFocusEvent *event) override {
        // imageLabel->setVisible(false); // 根据按钮状态显示或隐藏图片
        // setChecked(false); // 失去焦点时取消选中
        // textLabel->setStyleSheet("font-size: 16px; border: none; background-color: transparent;"); // 回复原样
        QPushButton::focusOutEvent(event); // 继续处理事件
    }
public:
    void changeStyle(bool checked) {
        imageLabel->setVisible(checked); // 根据按钮状态显示或隐藏图片
        if (checked) {
            textLabel->setStyleSheet("background-color: #EB3F5E; font-size: 16px; border-radius: 16px;"); // 选中时背景色
        } else {
            textLabel->setStyleSheet("background-color: transparent; font-size: 16px; border-radius: 16px;"); // 未选中时背景色
        }
    }

private slots:
    void updateStyle(bool checked) {
        imageLabel->setVisible(checked); // 根据按钮状态显示或隐藏图片
        if (checked) {
            textLabel->setStyleSheet("background-color: #EB3F5E; font-size: 16px; border-radius: 16px;"); // 选中时背景色
        } else {
            textLabel->setStyleSheet("background-color: transparent; font-size: 16px; border-radius: 16px;"); // 未选中时背景色
        }
    }

private:
    QLabel *textLabel;
    QLabel *imageLabel;
};

#endif //HORINAVIBUTTON_H
