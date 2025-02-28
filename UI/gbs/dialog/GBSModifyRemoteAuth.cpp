#include <QClipboard>
#include "GBSModifyRemark.h"
#include "ui_GBSModifyRemark.h"
#include "gbs/GBSMainCollector.h"
#include "gbs/common/GBSHttpClient.h"
#include "gbs/dto/GBSBundleData.h"
GBSModifyRemark::GBSModifyRemark(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GBSModifyRemark)
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

	GBSLiveAccountInfo account = GBSMainCollector::getInstance()->getAccountInfo();
	int liveDeviceId = account.getLiveDeviceId();

	std::string liveDeviceName = account.getDeviceName();
	std::string liveAccountId = account.getLiveAccount();
	std::string remoteAccount = account.getToDeskAccount();
	std::string remotePassword = account.getToDeskPassword();
	int remoteSwitch = account.getRemoteSwitch();
	QString remark = ui->lineEdit->text() + "/" + ui->lineEdit_2->text();
	GBSHttpClient::getInstance()->modifyZlmLiveDevice(liveDeviceName, liveDeviceId, "", remark.toStdString(),
							  liveAccountId,
							  remoteSwitch, remoteAccount, remotePassword);
    

}

GBSModifyRemark::~GBSModifyRemark()
{
    delete ui;
}


void GBSModifyRemark::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
		event->accept();
	}
	QDialog::mousePressEvent(event);
}

void GBSModifyRemark::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton) {
		move(event->globalPosition().toPoint() - dragPosition);
		event->accept();
	}
	QDialog::mouseMoveEvent(event);
}
