#include <QDialog>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QFontMetrics>

class CountdownDialog : public QDialog {
    Q_OBJECT

public:
    CountdownDialog(const QVector<int>& countdownValues, QWidget* parent = nullptr)
        : QDialog(parent), countdownArray(countdownValues), currentIndex(0) {

        // 设置对话框背景和外观
        setWindowTitle("倒计时对话框");
        setFixedSize(300, 250);  // 设置窗口大小
        setStyleSheet("background-color: black;");



        // 设置计时器
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &CountdownDialog::onTimeout);
        timer->start(1000); // 每1000ms即1秒触发一次


        // 设置阴影效果
        QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect();
        shadowEffect->setBlurRadius(20);
        shadowEffect->setOffset(5, 5);
        shadowEffect->setColor(Qt::black);
        setGraphicsEffect(shadowEffect);
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // 绘制圆形背景
        QRectF circleRect(50, 50, 200, 200); // 圆形的位置和大小
        painter.setBrush(Qt::white);
        painter.drawEllipse(circleRect);

        // 计算字体大小，使数字填充整个圆形
        int circleDiameter = circleRect.width();
        int fontSize = circleDiameter * 0.6;  // 字体大小为圆形直径的60%，可以调整比例
        QFont font = painter.font();
        font.setPointSize(fontSize);
        font.setBold(true);  // 设置字体为加粗
        painter.setFont(font);
        painter.setPen(Qt::black);  // 设置字体颜色为黑色

        // 获取倒计时数字并计算其在圆形中的居中位置
        QString countdownText = QString::number(countdownArray[currentIndex]);
        QFontMetrics metrics(font);
        QRect textRect = metrics.boundingRect(countdownText);

        // 获取圆形的中心点
        int centerX = (circleRect.left() + circleRect.right()) / 2;
        int centerY = (circleRect.top() + circleRect.bottom()) / 2;

        // 计算文本的水平位置，使其水平居中
        int x = centerX - textRect.width() / 2;

        // 计算文本的垂直位置，使其基线对齐圆形的底部
        int y = circleRect.bottom() - textRect.height() * 0.2;  // 将文本底部与圆形底部对齐，0.3是微调系数

        // 绘制倒计时数字
        painter.drawText(x, y, countdownText);
    }

private slots:
    void onTimeout() {
        // 更新倒计时数字
        currentIndex++;
        if (currentIndex < countdownArray.size()) {
            update();  // 强制重绘，更新倒计时数字
        } else {
            // 倒计时结束，关闭对话框
            timer->stop();
            accept();  // 关闭对话框
        }
    }

private:
    QVector<int> countdownArray;
    int currentIndex;
    QPushButton* closeButton;
    QTimer* timer;
};