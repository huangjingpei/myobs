#include "GBSBizSettingTimbreSquare.h"
#include "ui_GBSBizSettingTimbreSquare.h"


#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QGridLayout>
#include <QLabel>

class ImageButtons : public QWidget {


public:
    ImageButtons(QWidget *parent = nullptr) : QWidget(parent), currentPage(0), buttonsPerPage(18), buttonsPerRow(9), buttonCount(0), pageCount(0) {
        QVBoxLayout *vertLayout = new QVBoxLayout(this);
        QHBoxLayout *mainLayout = new QHBoxLayout(this);
        QHBoxLayout *pageLayout = new QHBoxLayout(this);

        // 创建翻页按钮
        lblPage = new QLabel(this);
        QPushButton *btnleft = new QPushButton(this);
        QPushButton *btnRight = new QPushButton(this);
        connect(btnRight, &QPushButton::clicked, this, &ImageButtons::onRight);
        connect(btnleft, &QPushButton::clicked, this, &ImageButtons::onLeft);
     

        // 创建 QStackedWidget 来存放各页按钮
        stackedWidget = new QStackedWidget(this);


        // 将翻页按钮放到主布局中
        mainLayout->addWidget(stackedWidget);  // 左边放按钮页面
        pageLayout->setAlignment(Qt::AlignHCenter);
        pageLayout->addWidget(btnleft);
        pageLayout->addSpacing(40);
        pageLayout->addWidget(btnRight);
        pageLayout->addWidget(lblPage);
        vertLayout->addLayout(mainLayout);
        vertLayout->addLayout(pageLayout);

        btnRight->setStyleSheet(
            "QPushButton {"
            "   background-image: url(:gbs/images/gbs/biz/gbs-arrow-right-20px.png);"
            "   background-repeat: no-repeat;"
            "   background-position: center;"
            "   color: white;"
            "   border: none;" // 无边框
            "   border-radius: 5px;" // 圆角
            "   font-size: 16px;"
            "   padding: 0;" // 不添加内边距
            "}"
            "QPushButton:pressed {"
            "   background-color: #D1D8DD;" // 按下时背景颜色
            "   padding-left: 3px;"  // 向左移动 3px
            "   padding-top: 3px;"    // 向上移动 3px
            "   background-repeat: no-repeat;"
            "   background-position: center;"
            "}"
        );
        btnleft->setStyleSheet(
            "QPushButton {"
            "   background-image: url(:gbs/images/gbs/biz/gbs-arrow-left-20px.png);"
            "   background-repeat: no-repeat;"
            "   background-position: center;"
            "   color: white;"
            "   border: none;" // 无边框
            "   border-radius: 5px;" // 圆角
            "   font-size: 16px;"
            "   padding: 0;" // 不添加内边距
            "}"
            "QPushButton:pressed {"
            "   background-color: #D1D8DD;" // 按下时背景颜色
            "   padding-left: 3px;"  // 向左移动 3px
            "   padding-top: 3px;"    // 向上移动 3px
            "   background-repeat: no-repeat;"
            "   background-position: center;"
            "}"
        );

        //setLayout(vertLayout);
    }

    QHBoxLayout* createButtonLabelLayout(const QString &buttonText, const QString &labelText) {
        QHBoxLayout *layout = new QHBoxLayout;

        QPushButton *button = new QPushButton(buttonText);
        button->setFixedSize(108, 130); // 设置按钮大小

        QLabel *label = new QLabel(labelText);

        layout->addWidget(button);
        layout->addWidget(label);
        layout->setAlignment(Qt::AlignLeft);

        return layout;
    }
private slots:
    // 翻页逻辑
    void onLeft() {
        int currentIndex = stackedWidget->currentIndex();
        if (currentIndex > 0) {
            stackedWidget->setCurrentIndex(--currentIndex);
        }
        lblPage->setText(QString("%1/%2 页").arg(currentIndex+1).arg(pageCount));
    }

    void onRight() {
        int currentIndex = stackedWidget->currentIndex();
        if (currentIndex < pageCount -1) {
            stackedWidget->setCurrentIndex(++currentIndex);
        }
        lblPage->setText(QString("%1/%2 页").arg(currentIndex+1).arg(pageCount));

    }

    void showHelp() {

    }

private:
    QLabel *lblPage;
    QStackedWidget *stackedWidget;
    int currentPage;    // 当前页数
    const int buttonsPerPage;  // 每页的按钮数
    const int buttonsPerRow;   // 每行按钮数量
    int buttonCount;
    int pageCount;

public:
    void addButton(QString path) {
        // 如果当前页面已经满了，创建新页面
        if (buttonCount % buttonsPerPage == 0) {
            addButtonPage();  // 创建新页面
            stackedWidget->setCurrentIndex(pageCount);  // 切换到新创建的页面
            pageCount++;  // 增加页面计数
        }

        // 获取当前页面
        QWidget* currentPage = stackedWidget->currentWidget();
        qDebug() << "page count " << pageCount << " button count " << buttonCount << " currentPage:" << currentPage;

        if (currentPage) {
            // 添加新的按钮到当前页面
            QPushButton* button = new QPushButton();
            int row = (buttonCount % buttonsPerPage) / buttonsPerRow;
            int col = (buttonCount % buttonsPerPage) % buttonsPerRow;

	
        
	button->setFixedSize(108, 131);

        

		
	    button->repaint();
            //button->setIcon(QPixmap(path).scaled(108, 131, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            // 当按钮被按下时
            connect(button, &QPushButton::pressed, [=]() {
		    button->setStyleSheet(QString(
                    "QPushButton {"
		    "   border-image: url(%1);"
                    "   background-color: #2667FE;"
                    "   border: none;" // 无边框
                    "   border-radius: 3px;" // 圆角
                    "   font-size: 20px;"
                    "   text-align: center;"
                    "}"
                    ).arg(path));

            });

            // 当按钮释放时，隐藏小图片
            connect(button, &QPushButton::released, this, [=]() {
		button->setStyleSheet(QString(
                    "QPushButton {"
		    "   border-image: url(%1);"
                    "   background-color: #E4ECFF;"
                    "   border: none;" // 无边框
                    "   border-radius: 3px;" // 圆角
                    "   font-size: 20px;"
                    "   text-align: center;"
                    "}"
                    ).arg(path));
            });


	     button->setStyleSheet(QString("QPushButton {"
					  "   border-image: url(%1);"
					  "   background-repeat: no-repeat;"
					  "   background-position: center;"
					  "   color: white;"
					  "   border: none;"       // 无边框
					  "   border-radius: 5px;" // 圆角
					  "   font-size: 16px;"
					  "   padding: 0;" // 不添加内边距
					  "}"
					  "QPushButton:pressed {"
					  "   background-color: #D1D8DD;" // 按下时背景颜色
					  "   padding-left: 3px;"         // 向左移动 3px
					  "   padding-top: 3px;"          // 向上移动 3px
					  "   background-repeat: no-repeat;"
					  "   background-position: center;"
					  "}")
					  .arg(path));

            QGridLayout *layout = qobject_cast<QGridLayout*>(currentPage->layout());
            if (layout) {
                layout->addWidget(button, row, col);  // 添加按钮到布局
            }

        }

        buttonCount++;  // 计数加一
	lblPage->setText(QString("%1/%2 页").arg(pageCount).arg(pageCount));

    }

    void addButtonPage() {
        QWidget *pageWidget = new QWidget(this);  // 创建一个新的页面
        QGridLayout *gridLayout = new QGridLayout(pageWidget);  // 设置网格布局

        // 设置按钮间距和布局边距
        gridLayout->setHorizontalSpacing(10);
        gridLayout->setVerticalSpacing(10);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setSpacing(10);

        // 将新页面添加到 QStackedWidget 中
        pageWidget->setLayout(gridLayout);

        // 添加 QSpacerItem 来确保按钮排列在上方，空白区域填充在下面
        QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(spacer, buttonsPerPage / buttonsPerRow, 0, 1, buttonsPerRow);  // 在最后一行添加 SpacerItem

        stackedWidget->addWidget(pageWidget);

    }

};



GBSBizSettingTimbreSquare::GBSBizSettingTimbreSquare(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GBSBizSettingTimbreSquare)
{
    ui->setupUi(this);

    ui->tabWidget->setStyleSheet(
        "QTabWidget::pane {"
        "    border: none;"  // 移除tab pane的边框
        "}"
        "QTabBar::tab {"
        "    background: none;"  // tab的背景颜色
        "    padding: 10px;"  // tab内容的填充
        "    margin-right: 10px;"  // 调整tab之间的水平间距
        "    border: none;"  // 移除tab的边框
        "    color: #01C667;"
        "    font-size:16px;"
        "}"
        "QTabBar::tab:first {"
        "    margin-left: 100px;"  // 调整第一个tab项的左外边距
        "}"
        "QTabBar::tab:selected {"
        "    background: #C0C0C0;"  // 选中tab的背景颜色
        "    border: none;"  // 选中时也不显示边框
        "    border-radius: 16px"
        "}"
        "QTabBar {"
        "    qproperty-alignment: AlignCenter;"  // 设置tab项居中对齐
        "}"
        );

    QString comboBoxStyleSheet = R"(
        QComboBox {
        border: 1px solid gray;
        border-radius: 3px;
        padding: 5px;
        }

        /* 设置 QComboBox 的箭头颜色和大小 */
        QComboBox::down-arrow {
        image: none;  /* 取消默认图标 */
        border: none;
        width: 20px;   /* 设置箭头宽度 */
        height: 20px;  /* 设置箭头高度 */
        image: url(:gbs/images/gbs/biz/gbs-arrow-down-green.png);  /* 替换为你想要的箭头图片 */

        }

        QComboBox::down-arrow:enabled {
        border: none;
        image: url(:gbs/images/gbs/biz/gbs-arrow-down-green.png);  /* 替换为你想要的箭头图片 */
        }



     //    /* 调整 QComboBox 的下拉按钮 */
     //    QComboBox::drop-down {
        // width: 25px;  /* 控制下拉按钮的宽度 */
        // border: none;
        // background-color: #f0f0f0;
     //    }
    )";

    ui->comboBox->setStyleSheet(comboBoxStyleSheet);
    ui->comboBox_2->setStyleSheet(comboBoxStyleSheet);

    ImageButtons* imgButtons = new ImageButtons();

    for (int i = 0 ;i < 40; i++) {
        imgButtons->addButton(":gbs/images/gbs/biz/gbs-model.png");
    }


    ui->verticalLayout->addWidget(imgButtons);

    QLabel *label = new QLabel("他的音色风格");
    ui->verticalLayout->addWidget(label);

    ImageButtons* imgButtons2 = new ImageButtons();

    for (int i = 0 ;i < 30; i++) {
        imgButtons2->addButton(":gbs/images/gbs/biz/gbs-emoj.png");
    }

    ui->verticalLayout->addWidget(imgButtons2);

    
    connect(ui->lineEdit, &QLineEdit::textChanged, this, [this](const QString &text) {
	    if (text.isEmpty()) {
		    ui->pushButton_4->setVisible(true);
	    } else {
		    ui->pushButton_4->setVisible(false);
	    }
	    
	    });
}

GBSBizSettingTimbreSquare::~GBSBizSettingTimbreSquare()
{
    delete ui;
}
