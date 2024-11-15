#ifndef VERTNAVIBUTTON_H
#define VERTNAVIBUTTON_H
#include <QAbstractButton>
#include <QIcon>
#include <QPainter>
#include <QStyleOptionButton>
#include <QPushButton>
#include <QEvent>
#include <QEnterEvent>
#include <QIcon>

/*
悬停状态的图标：在 QPushButton:hover 中通过 qproperty-icon 设置为白色版本的图标。
    "   qproperty-icon: url(:/Resources/hover-icon-white.png);"  // 悬停状态的白色图标
按下状态的图标：在 QPushButton:pressed 中也通过 qproperty-icon 设置为按下状态的白色图标。
    "   qproperty-icon: url(:/Resources/pressed-icon-white.png);"  // 按下状态的白色图标
*/


class VertNaviButton : public QPushButton
{
    Q_OBJECT

public:
    explicit VertNaviButton(const QString &text, const QString &iconPath, QWidget *parent = nullptr)
        : QPushButton(parent), icon(iconPath), pressedIcon(changeIconFile(iconPath, "-transparent")), currentIcon(&icon)
    {
        qDebug() << "iconPath " << iconPath << " transparent path " << changeIconFile(iconPath, "-transparent");
   //          QString myStyle = R"(
   //                  QPushButton {
   //                      border: 2px solid #1B2846;
   //                      border-radius: 10px;
   //                      background-color: qlineargradient(
   //                          spread:pad,
   //                          x1:0, y1:0, x2:1, y2:1,
   //                          stop:0.1054 #1B2846,
   //                          stop:0.9062 #000000
   //                      );
   //                      color: white; /* 设置文本颜色 */
   //                      padding: 10px; /* 内边距 */
   //                  }



         //    QPushButton:hover {
            // background-color: qlineargradient(
            //     spread:pad,
            //     x1:0, y1:0, x2:1, y2:1,
            //     stop:0.1054 #2C3956,
            //     stop:0.9062 #111111
            // );
            // background-color: #1B2846;

         //    }

   //                  QPushButton:pressed {
   //                     background-color: #F9F9F9; // 按下状态背景颜色
   //                     padding-left: 1px;  /* 向左移动 3px */"
   //                     padding-top: 1px;    /* 向上移动 3px */"
   //                     color: white;  // 按下状态字体颜色
   //                  }
   //              )";
   //          setStyleSheet(myStyle);
        setStyleSheet(
            "QPushButton {"
            "   background-color: transparent;"
            "   border: #000000;" // 无边框
            "   border-radius: 5px;" // 圆角
            //"   font-size: 14px;"
            //"   padding-left: 70px;" // 调整文本位置
            //"   text-align: left;" // 左对齐文本
            "}"
            // "QPushButton:hover {"
            // "   background-color: #1B2846;"
            // "   color: white;"  // 悬停状态字体颜色

            // "}"
            // "QPushButton:pressed {"
            // "   background-color: #1B2846;" // 按下状态背景颜色
            // "   padding-left: 1px;  /* 向左移动 3px */"
            // "   padding-top: 1px;    /* 向上移动 3px */"
            // "   color: white;"  // 按下状态字体颜色
            // "   padding-left: 70px;" // 调整文本位置
            // "   text-align: left;" // 左对齐文本
            // "}"
            );
            setLayoutDirection(Qt::LeftToRight);
            setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
            setIconSize(QSize(22, 22));  // 设置图标大小
            //setText(text);
            rightText = text;

        setMouseTracking(true);
        setCheckable(true);
        setAttribute(Qt::WA_Hover, true);  // 启用悬停事件


    }
protected:

    // void enterEvent(QEnterEvent *event) override {
    //     Q_UNUSED(event);
    //     currentIcon = &pressedIcon;

    //     qDebug() << "enterEvent" << currentIcon;
    // }

    // void leaveEvent(QEvent *event) override {
    //     Q_UNUSED(event);
    //     currentIcon = &icon;
    //     qDebug() << "leaveEvent" << currentIcon;

    // }
private:
    QString changeIconFile(const QString &origin, const QString& replace) {
        int lastDotIndex = origin.lastIndexOf('.');
        if (lastDotIndex == -1) {
            // 如果没有找到点，返回一个空字符串或者错误信息
            return QString();
        }
        QString fileNameWithoutExtension = origin.left(lastDotIndex);
        QString fileExtension = origin.mid(lastDotIndex);

        return fileNameWithoutExtension + replace + fileExtension;
    }

public:
    void changeStyle(bool checked) {
            markFlag = checked;
        if (checked) {
            currentIcon = &pressedIcon;
        } else {
            currentIcon = &icon;
        }
    }

protected:
    void paintEvent(QPaintEvent *event) override {

            // 调用父类的 paintEvent 来处理默认的绘制
        QPushButton::paintEvent(event);

        // 创建绘制器
        QPainter painter(this);

        // 启用抗锯齿渲染提示，减少锯齿效果
        painter.setRenderHint(QPainter::Antialiasing, true);             // 图形抗锯齿
        painter.setRenderHint(QPainter::TextAntialiasing, true);         // 文本抗锯齿
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);    // 平滑缩放图像


        // 设置背景颜色
        if (markFlag) {
            painter.setBrush(QColor("#1B2846"));
            painter.setPen(Qt::NoPen);  // 去除边框
            painter.drawRect(rect());   // 绘制背景


        }
        // 显示左侧白色矩形
        if (markFlag) {

            painter.setBrush(QBrush(Qt::white));
            painter.setPen(Qt::NoPen);  // 无边框

            // 绘制左侧的矩形，宽度为 20px，高度为按钮的 3/4，高度居中
            int rectHeight = height() * 3 / 4;
            int rectY = (height() - rectHeight) / 2;
            painter.drawRect(0, rectY, 5, rectHeight);
        }

        // 在左边距离40px的位置绘制icon
        int iconX = 15; // 左边距离
        int iconY = (height() - iconSize().height()) / 2;  // 垂直居中
        QPixmap pixmap = currentIcon->pixmap(iconSize());
        painter.drawPixmap(iconX, iconY, pixmap);

        // 添加文本标签
        QString text = rightText;  // 要显示的文本内容
        QFont font = painter.font();
        font.setPointSize(12);  // 设置文本大小

        if (markFlag) {
            painter.setBrush(QColor("#FFFFFF"));
            painter.setFont(font);
            painter.setPen(Qt::white);  // 设置文本颜色为白色
        } else {
            painter.setBrush(QColor("#78828A"));
            painter.setFont(font);
            painter.setPen(QColor("#78828A"));  // 设置文本颜色为白色
        }

        int textX = iconX + pixmap.width() + 10;  // 文本左侧与图标右侧保持 10px 距离
        int textY = (height() + font.pointSize()) / 2;  // 垂直居中显示
        painter.drawText(textX, textY, text);

    }


    // 按钮点击状态的变化触发重绘
    void mousePressEvent(QMouseEvent *event) override {
        QPushButton::mousePressEvent(event);
        update();
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        QPushButton::mouseReleaseEvent(event);
        update();
    }


private:
    QIcon icon;
    QIcon pressedIcon;

    QIcon* currentIcon = nullptr;
    bool markFlag = false;
    QString rightText;

};

#endif
