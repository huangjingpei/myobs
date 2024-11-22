#ifndef __Q_TOAST_H__
#define __Q_TOAST_H__
#include <QApplication>
#include <QWidget>
#include <QLineEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

class QToast : public QWidget {
    Q_OBJECT

public:
    explicit QToast(QWidget *parent = nullptr, const QString &message = "", int duration = 2000)
        : QWidget(parent) {
        setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_ShowWithoutActivating);

        // 设置布局和消息
        QVBoxLayout *layout = new QVBoxLayout(this);
	QLineEdit *textEdit = new QLineEdit(message, this);
	
	textEdit->setFixedSize(1200, 60);
	textEdit->setReadOnly(true); // 设置为只读
	textEdit->setAlignment(Qt::AlignCenter);
	textEdit->setStyleSheet("color: black; font-size: 32px; word-wrap: break-word; border:none;alignment: center;");
	layout->addWidget(textEdit, 0, Qt::AlignHCenter);

        setStyleSheet("background-color: rgba(0, 0, 0, 180); border-radius: 4px;");
	setFixedSize(1200, 60);

        // 居中显示
        if (parent) {
            setGeometry(parent->geometry().center().x() - width() / 2,
                        parent->geometry().center().y() - height() / 2, 
                        width(), 
                        height());
		qDebug() << "x " << parent->geometry().center().x();
	    qDebug() << "width " << width();
		qDebug() << "y " << parent->geometry().center().y();
	    qDebug() << "height " << height();
        } else {
		QScreen *screen = QGuiApplication::primaryScreen();
		move((screen->geometry().width() - width()) / 2, (screen->geometry().width() - height()) / 2);
        }

        // 动画效果
        QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(this);
        setGraphicsEffect(effect);
        QPropertyAnimation *fadeOut = new QPropertyAnimation(effect, "opacity");
        fadeOut->setDuration(duration);
        fadeOut->setStartValue(1.0);
        fadeOut->setEndValue(0.0);

        // 自动关闭
        QTimer::singleShot(duration, this, &QToast::close);
        connect(fadeOut, &QPropertyAnimation::finished, this, &QToast::close);

        fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
    }
};


#endif //__Q_TOAST_H__
