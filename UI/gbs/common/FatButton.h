#ifndef FATBUTTON_H
#define FATBUTTON_H
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

class FatButton : public QPushButton {
    Q_OBJECT

public:
    FatButton(QWidget *parent = nullptr) : FatButton("", "", parent) {

    }
    FatButton(const QString &imagePath, const QString &text, QWidget *parent = nullptr)
        : QPushButton(parent), m_text(text) {
        // 设置按钮的固定大小
        setFixedSize(150, 80);
        // 加载图片
        //m_image.load(imagePath);
		
		setCheckable(true);  // 允许按钮被选中
		connect(this, &QPushButton::toggled, this, &FatButton::updateStyle);


        qDebug() << "constructor " << "FatButton";


        layout = new QVBoxLayout(this);
        setStyleSheet("background-color:#FFFFFF;border: 1px solid #D7D4D4;");

        setLayout(layout);

    }

	void setImgAndTxt(const QString &imagePath, const QString &imagePressedPath, const QString &text) {
        qDebug()<< "setImgAndTxt" << imagePath;
		m_image.load(imagePath);
        m_color_image.load(imagePressedPath);
		m_text = text;
		// 触发重绘

        imageLabel = new QLabel(this);
        imageLabel->setStyleSheet("border:none;");
        imageLabel->setPixmap(m_image.scaled(60, 40, Qt::KeepAspectRatio)); // 调整图片大小
        textLabel = new QLabel(m_text, this);
        setStyleSheet("background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #1B2846, stop: 1 #000000);"
                      "border: 1px solid #D7D4D4;");
        textLabel->setStyleSheet("border:none;"
                                 "font-size: 12px;" // 字体大小
                                 "color:#F9F9F9;"
                                 );
        textLabel->setAlignment(Qt::AlignHCenter | Qt::AlignBottom); // 文本居中

        // 将图像和文本添加到布局中
        layout->addWidget(imageLabel,0, Qt::AlignHCenter);
        layout->addWidget(textLabel,0, Qt::AlignHCenter);
    	update();
	}


// protected:
//     void paintEvent(QPaintEvent *event) override {
//         Q_UNUSED(event);
//         qDebug() << "constructor " << "paintEvent";


//         QPainter painter(this);

//         // 计算图片的位置，确保图片居中
//         int imageX = (width() - m_current_img.width()) / 2; // X 坐标
//         int imageY = 0; // Y 坐标为 0

//         // 绘制上半部分的图片
//         painter.drawPixmap(imageX, imageY, m_current_img);

//         // 设置文本样式
//         painter.setFont(QFont("Arial", 12));
//         painter.setPen(Qt::black);
//         // 计算文本的绘制位置
//         QRect textRect(0, 60, width(), 20);
//         painter.drawText(textRect, Qt::AlignCenter, m_text);
//     }


private slots:
    void updateStyle(bool checked) {
		
        if (!checked) {
            imageLabel->setPixmap(m_image.scaled(60, 40, Qt::KeepAspectRatio)); // 调整图片大小
            imageLabel->setStyleSheet("border:none;");
            setStyleSheet("background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #1B2846, stop: 1 #000000);"
                          "border: 1px solid #D7D4D4!important;");
            textLabel->setStyleSheet("border:none;"
                                     "font-size: 12px;" // 字体大小
                                     "color:#F9F9F9;"
                                     );
        } else {
            imageLabel->setPixmap(m_color_image.scaled(60, 40, Qt::KeepAspectRatio)); // 调整图片大小
            imageLabel->setStyleSheet("border:none;");
            setStyleSheet("background-color:#FFFFFF;border: 1px solid #D7D4D4!important;");
            textLabel->setStyleSheet("border:none;"
                                     "font-size: 12px;" // 字体大小
                                     "color:#D7D4D4;"
                                     );
        }
    }


private:
    QPixmap m_image;
    QPixmap m_color_image;
    QString m_text;
    QVBoxLayout* layout;
    QLabel *imageLabel;
    QLabel *textLabel;
};


#endif //FATBUTTON_H
