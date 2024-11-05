#include "GBSMsgDialog.h"
#include "ui_GBSMsgDialog.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QDialog>
#include <QWidget>
#include <QPushButton>

GBSMsgDialog::GBSMsgDialog(QString title, QLayout* layout, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GBSMsgDialog)
{
    ui->setupUi(this);
    setFixedSize(815, 453);
    //setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    qDebug() <<"width " << width() << " height " <<height();

    QWidget *contentWidget = new QWidget();
    contentWidget->setLayout(layout);
    contentWidget->setGeometry(0, 54, width(), height() - 54); // 设置内容区域的位置和大小

    QVBoxLayout *mainLayout =new QVBoxLayout();
    QHBoxLayout *horiLayout = new QHBoxLayout();
    horiLayout->setGeometry(QRect{0, 0, width(), 54});

    QPushButton* btnFavIcon = new QPushButton();
    btnFavIcon->setFixedSize(48, 48);
    btnFavIcon->setGeometry(0, ((height() - 48))/2, 48, 48);
    btnFavIcon->setStyleSheet(
        "QPushButton {"
        "   background-image: url(:gbs/images/gbs/favicon/favicon-48.ico);"
        "   background-repeat: no-repeat;"
        "   background-position: center;"
        "   color: white;"
        "   border: none;" // 无边框
        "   border-radius: 999px;" // 圆角
        "   font-size: 16px;"
        "   padding: 0;" // 不添加内边距
        "}"

        );

    QLabel *label = new QLabel(title);


    QPushButton *btnClose = new QPushButton();
    btnClose->setFixedSize(48, 48); // 设置按钮大小
    btnClose->setStyleSheet("QPushButton {"
                          "   background-image: url(:gbs/images/gbs/biz/gbs-close-window);"
                          "  border-radius: 5px;"       // 圆角
                          "}"
                          "QPushButton:hover {"
                          "  border: none;"
                          "   background-color: #F9F9F9;"
                          "}"
                          "QPushButton:pressed {"
                          "   background-color: #D1D8DD;" // 按下时背景颜色
                          "   padding-left: 3px;"  // 向左移动 3px
                          "   padding-top: 3px;"    // 向上移动 3px
                          "   background-repeat: no-repeat;"
                          "   background-position: center;"
                          "}"
                          );

    int btnX = width() - btnClose->width() - 20;
    btnClose->setGeometry(btnX, (54 - btnClose->height()) / 2, btnClose->width(), btnClose->height());
    qDebug() << "btnX " << btnX;

    horiLayout->addWidget(btnFavIcon);
    horiLayout->addWidget(label);
    horiLayout->addWidget(btnClose);
    mainLayout->addLayout(horiLayout);
    mainLayout->addWidget(contentWidget);



    // 设置渐变背景色
    contentWidget->setStyleSheet(
        "QWidget { "
        "background: qlineargradient("
        "spread:pad, "
        "x1:0, y1:0, x2:1, y2:1, "
        "stop:0.1543 rgba(237, 255, 246, 255), "
        "stop:0.822 rgba(255, 255, 255, 255)"
        "); "
        "border: none; "


        "}"
        );

    if (title.isEmpty()) {

    } else {
        if (layout != nullptr) {

            setWindowTitle(title);
            setLayout(mainLayout);
        }
    }

    connect(btnClose, &QPushButton::clicked, this, &QDialog::close);
  //  connect(ui->push)

}

GBSMsgDialog::~GBSMsgDialog()
{
    delete ui;
}


void GBSMsgDialog::resizeEvent(QResizeEvent *event) {
    QDialog::resizeEvent(event);
    // 更新关闭按钮的位置
    QLabel *label = findChild<QLabel *>();
    QPushButton *btnClose = findChild<QPushButton *>();

    if (label && btnClose) {
        int btnX = label->width() - btnClose->width() - 20;
        btnClose->setGeometry(btnX, (label->height() - btnClose->height()) / 2, btnClose->width(), btnClose->height());
    }
}
