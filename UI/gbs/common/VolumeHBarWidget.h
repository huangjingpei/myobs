#ifndef VOLUMEHBARWIDGET_H
#define VOLUMEHBARWIDGET_H
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QStyle>
#include <QPixmap>

class VolumeHBarWidget : public QWidget {
    Q_OBJECT

public:
    explicit VolumeHBarWidget(const QString &sourceName, const QString &iconPath, QWidget *parent = nullptr)
            : QWidget(parent), volumeBar(new QProgressBar(this)) {

	    QVBoxLayout *mainLayout = new QVBoxLayout(this);
	    enable = 1;

	    // 调整垂直布局的外边距和行间距
	    mainLayout->setContentsMargins(0, 0, 0, 0);  // 外边距设为 0
	    mainLayout->setSpacing(2);  // 行间距设为 2 像素，进一步减少

	    // 第一行：声音来源的标签
	    QLabel *sourceLabel = new QLabel(sourceName, this);
	    sourceLabel->setStyleSheet("margin-bottom: 0px; padding: 0px;");  // 移除内外边距
	    mainLayout->addWidget(sourceLabel);

	    // 第二行：图标和音量条
	    QHBoxLayout *secondRowLayout = new QHBoxLayout();
	    secondRowLayout->setContentsMargins(0, 0, 0, 0);  // 去掉第二行布局的外边距
	    secondRowLayout->setSpacing(3);  // 调整图标和音量条之间的间距为 3

	    // 按钮
	    QPushButton* button = new QPushButton(this);
	    button->setFixedSize(40, 40); // 设置按钮大小
	    QString styleSheet = QString(
					 "QPushButton {"
					 "   border: none;"  // 移除边框
					 "   background-image: url(%1);"  // 使用iconPath作为背景图片路径
					 "   background-repeat: no-repeat;"
					 "   background-position: center;"
					 "   padding: 0px;"  // 去掉内边距
					 "}"
					 "QPushButton:hover {"
					 "   background-color: #F9F9F9;"
					 "}"
					 "QPushButton:pressed {"
					 "   padding-left: 3px;"  // 向左移动 3px
					 "   padding-top: 3px;"    // 向上移动 3px
					 "}"
					 ).arg(iconPath);
	    button->setStyleSheet(styleSheet);
	    secondRowLayout->addWidget(button);

	    // 自定义音量条（水平进度条）
	    volumeBar->setFixedHeight(5);
	    volumeBar->setRange(0, 100);  // 范围从 0 到 100
	    volumeBar->setValue(50);  // 默认值为 50%
	    volumeBar->setOrientation(Qt::Horizontal);  // 水平进度条
	    volumeBar->setTextVisible(false);  // 隐藏文本

	    // 设置音量条样式
	    volumeBar->setStyleSheet(
		    "QProgressBar {"
		    "   border: none;"
		    "   background-color: #e0e0e0;"  // 进度条背景颜色
		    "   height: 5px;"  // 进度条高度
		    "   width: 180px;"  // 进度条宽度
		    "   border-radius: 5px;"  // 圆角半径
		    "   margin: 0px;"  // 去掉进度条的外边距
		    "   padding: 0px;"  // 去掉进度条的内边距
		    "}"
		    "QProgressBar::chunk {"
		    "   background-color: #00C566;"  // 进度条填充颜色
		    "   border-radius: 10px;"  // 圆角半径
		    "}"
		    );

	    // 将音量条添加到第二行布局
	    secondRowLayout->addWidget(volumeBar);

	    // 将第二行布局添加到主布局
	    mainLayout->addLayout(secondRowLayout);

            connect(button, &QPushButton::clicked, this, &VolumeHBarWidget::onButtonClicked);
    }
    signals:
    void toggleButton(bool toggle, intptr_t ptr);

public slots:
    void onButtonClicked() {
            enable = (enable == 0) ? 1 :0;
            emit toggleButton(enable, (intptr_t)this);
    }

private:
    bool enable;
    QProgressBar *volumeBar;
};

#endif //VOLUMEHBARWIDGET_H
