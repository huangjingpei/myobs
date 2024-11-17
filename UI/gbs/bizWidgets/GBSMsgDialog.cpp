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
	ui->btnClose->setFixedSize(QSize{48, 48});
	ui->btnClose->setStyleSheet("QPushButton {"
				"   background-image: url(:gbs/images/gbs/biz/gbs-close-window.png);"
				"  border-radius: 5px;" // 圆角
				"}"
				"QPushButton:hover {"
				"  border: none;"
				"   background-color: #F9F9F9;"

				"}"
				"QPushButton:pressed {"
				"   background-color: #D1D8DD;" // 按下时背景颜色
				"   padding-left: 3px;"         // 向左移动 3px
				"   padding-top: 3px;"          // 向上移动 3px
				"   background-repeat: no-repeat;"
				"   background-position: center;"

				"}");

    if (!title.isEmpty()) {
	    ui->label->setText(title);
	    ui->label->setStyleSheet("QLabel { "
		    "font-size: 16px; "
		    "}");

    }

    // 设置渐变背景色
    ui->widget_2->setStyleSheet(
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

    ui->widget_2->setLayout(layout);


    connect(ui->btnClose, &QPushButton::clicked, this, &QDialog::close);
  

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


void GBSMsgDialog::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
		event->accept();
	}
	QDialog::mousePressEvent(event);
}

void GBSMsgDialog::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton) {
		move(event->globalPosition().toPoint() - dragPosition);
		event->accept();
	}
	QDialog::mouseMoveEvent(event);
}
