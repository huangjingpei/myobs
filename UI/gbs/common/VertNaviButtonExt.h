#ifndef VERTNAVIBUTTONEXT_H
#define VERTNAVIBUTTONEXT_H
#include <QAbstractButton>
#include <QIcon>
#include <QPainter>
#include <QStyleOptionButton>
#include <QPushButton>
#include <QEvent>
#include <QIcon>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
/*
悬停状态的图标：在 QPushButton:hover 中通过 qproperty-icon 设置为白色版本的图标。
    "   qproperty-icon: url(:/Resources/hover-icon-white.png);"  // 悬停状态的白色图标
按下状态的图标：在 QPushButton:pressed 中也通过 qproperty-icon 设置为按下状态的白色图标。
    "   qproperty-icon: url(:/Resources/pressed-icon-white.png);"  // 按下状态的白色图标
*/


class VertNaviButtonExt : public QPushButton
{
    Q_OBJECT

public:
    explicit VertNaviButtonExt(const QString &text, const QString &iconPath, QWidget *parent = nullptr)
        : QPushButton(parent), icon(iconPath)
    {
            QString myStyle = R"(
                    QPushButton {
                        border: 2px solid #1B2846;
                        border-radius: 5px;
                        background-color: qlineargradient(
                            spread:pad,
                            x1:0, y1:0, x2:1, y2:1,
                            stop:0.1054 #1B2846,
                            stop:0.9062 #000000
                        );
                        color: white; /* 设置文本颜色 */
                        padding: 5px; /* 内边距 */
                    }



		    QPushButton:hover {
			background-color: qlineargradient(
			    spread:pad,
			    x1:0, y1:0, x2:1, y2:1,
			    stop:0.1054 #2C3956,
			    stop:0.9062 #111111
			);
		    }
		)";
	    setStyleSheet(myStyle);

            // 创建布局
            QHBoxLayout *layout = new QHBoxLayout(this);
            layout->setDirection(QHBoxLayout::LeftToRight);

	    // 添加第一个按钮图标
	    QPushButton *iconButton = new QPushButton(this);
	    iconButton->setIcon(QIcon(iconPath));
	    iconButton->setIconSize(QSize(24, 24)); // 设置图标大小
	    iconButton->setFixedSize(24, 24); // 设置图标大小
	    iconButton->setFlat(true); // 无边框
	    layout->addWidget(iconButton);

	    // 添加文字标签
	    QLabel *textLabel = new QLabel(text, this);
	    textLabel->setStyleSheet("QLabel {"
					     "background-color: qlineargradient("
					     "spread:pad,"
						"      x1:0, y1:0, x2:1, y2:1,"
						"      stop:0.1054 #1B2846,"
						"      stop:0.9062 #000000"
					     ");"
			"   color: white;"
			"   font-size: 16px;"
			"   border-radius: 5px;" // 圆角
			"   border: none;" // 无边框
			"}"
		    );
	    layout->addWidget(textLabel);



	    btnOpen = new QPushButton(this);
	    btnOpen->setCursor(Qt::PointingHandCursor);  // 设置鼠标手型为可点击
	    btnOpen->setFixedSize(20, 20);  // 确保尺寸足够大
	    btnOpen->setStyleSheet("QPushButton {"
				   "background-image: url(:Resources/gbs-local-source-btn-open-dark.png);"
				   "   background-repeat: no-repeat;"
				   "   background-position: center;"
				   "   color: white;"
				   "   border: none;" // 无边框
				   "   border-radius: 5px;" // 圆角
				   "   font-size: 16px;"
				   "   padding: 0;" // 不添加内边距
				   "}"
				   "QPushButton:hover {"
				   "   background-color: #F9F9F9;"
				   "}"
				   "QPushButton:pressed {"
				   "   background-color: #D1D8DD;" // 按下时背景颜色
				   "   padding-left: 3px;"  // 向左移动 3px
				   "   padding-top: 3px;"    // 向上移动 3px
				   "   background-image: url(:/gbs/images/gbs/login/scan-qrcode-login.png);"
				   "   background-repeat: no-repeat;"
				   "   background-position: center;"
				   "}"
				   );
	    layout->addWidget(btnOpen);

	    btnClose = new QPushButton(this);
	    btnOpen->setCursor(Qt::PointingHandCursor);  // 设置鼠标手型为可点击
	    btnClose->setFixedSize(20, 20);  // 确保尺寸足够大
	    btnClose->setStyleSheet("QPushButton {"
				    "background-image: url(:Resources/gbs-local-source-btn-close-dark.png);"
				    "   background-repeat: no-repeat;"
				    "   background-position: center;"
				    "   color: white;"
				    "   border: none;" // 无边框
				    "   border-radius: 5px;" // 圆角
				    "   font-size: 16px;"
				    "   padding: 0;" // 不添加内边距
				    "}"
				    "QPushButton:hover {"
				    "   background-color: #F9F9F9;"
				    "}"
				    "QPushButton:pressed {"
				    "   background-color: #D1D8DD;" // 按下时背景颜色
				    "   padding-left: 3px;"  // 向左移动 3px
				    "   padding-top: 3px;"    // 向上移动 3px
				    "   background-image: url(:/gbs/images/gbs/login/scan-qrcode-login.png);"
				    "   background-repeat: no-repeat;"
				    "   background-position: center;"
				    "}"
				    );
	    layout->addWidget(btnClose);

	    // 设置布局的对齐方式
	    layout->setSpacing(5);
	    layout->setContentsMargins(0, 0, 0, 0);  // 设置边距
	    btnOpenToggled = false;
	    btnCloseToggled = false;
	    // 连接点击事件
	    connect(btnOpen, &QPushButton::clicked, this, &VertNaviButtonExt::onOpenClicked);
	    connect(btnClose, &QPushButton::clicked, this, &VertNaviButtonExt::onCloseClicked);

    }

    void setSwitch(bool use) {
            useSwitchButton = true;
    }
    void open() {

    }
    void close() {

    }

 signals:
    void openClicked();
    void closeClicked();

public slots:
    void onOpenClicked() {
            if (btnOpenToggled) {
                    btnClose->setStyleSheet("QPushButton {"
                                            "background-image: url(:Resources/gbs-local-source-btn-close-dark.png);");

	    } else {
		    btnClose->setStyleSheet("QPushButton {"
					    "background-image: url(:Resources/gbs-local-source-btn-close.png);");
	    }
	    btnOpenToggled = !btnOpenToggled;
	    emit openClicked();
    }

    void onCloseClicked() {
            if (btnCloseToggled) {
                    btnClose->setStyleSheet("QPushButton {"
                                            "background-image: url(:Resources/gbs-local-source-btn-open-dark.png);");

	    } else {
		    btnClose->setStyleSheet("QPushButton {"
					    "background-image: url(:Resources/gbs-local-source-btn-open.png);");
	    }
	    btnCloseToggled = !btnCloseToggled;
	    emit closeClicked();
    }



private:
    //Magic is always too powerful to be restrained.
    QPushButton* btnOpen;
    QPushButton* btnClose;
    bool btnOpenToggled;
    bool btnCloseToggled;
    QIcon icon;
    bool useSwitchButton;
};

#endif //VERTNAVIBUTTONEXT_H
