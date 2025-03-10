#include <QClipboard>
#include "GBSMsgConfirm.h"
#include "ui_GBSMsgConfirm.h"
#include "gbs/GBSMainCollector.h"
#include "gbs/common/GBSHttpClient.h"
#include "gbs/dto/GBSBundleData.h"
#include "gbs/GBSMainCollector.h"
GBSMsgConfirm::GBSMsgConfirm(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GBSMsgConfirm)
{
    ui->setupUi(this);
     setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    //setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

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
	"#myButton {"
	"background-color:#00C566;"
	"border: 1px solid white;"
	"border-radius:10px;"
	"font-size:20px;"
	"}"
	"#myButton:pressed {"
	"   padding-left: 3px;" // 向左移动 3px
	"   padding-top: 3px;"  // 向上移动 3px
	"   background-repeat: no-repeat;"
	"   background-position: center;"
	"}"
    );
    ui->widget->setStyleSheet("QWidget {"
			      "background-color:#0xFFFFFF;"
			      "background:#0xFFFFFF;"
			      "border:none;"
			      "}");
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

	connect(ui->btnClose, &QPushButton::clicked, this, [this]() { close(); });

	connect(ui->pushButton, &QPushButton::clicked, this, [this]() {

		QString text = ui->lineEdit->text();
		QString password =GBSMainCollector::getInstance()->getGuarderCtrlPassword();
		if (!text.isEmpty() && (text.compare(password) == 0)) {
			GBSMainCollector::getInstance()->getAccountInfo();
			GBSHttpClient::getInstance()->deletedSrsLiveDeviceV2(liveId);
			
		} else {
			ui->label_3->setText("密码错误");
		}
		});
	connect(ui->pushButton_2, &QPushButton::clicked, this, [this]() { close(); });

	GBSHttpClient::getInstance()->registerHandler(this);
}

GBSMsgConfirm::~GBSMsgConfirm()
{
	GBSHttpClient::getInstance()->unRegisterHandler(this);
	delete ui;
}


void GBSMsgConfirm::onDeletedMatrix(int code) {
	QMetaObject::invokeMethod(this, [code, this]() {
		if (code == 0) {
			if (liveWidget != nullptr) {
				liveWidget->removeRow(currentRow);
			}
			ui->label_3->setText("删除成功");
		} else {
			ui->label_3->setText("删除失败");
		}
		});

}


void GBSMsgConfirm::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
		event->accept();
	}
	QDialog::mousePressEvent(event);
}

void GBSMsgConfirm::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton) {
		move(event->globalPosition().toPoint() - dragPosition);
		event->accept();
	}
	QDialog::mouseMoveEvent(event);
}
