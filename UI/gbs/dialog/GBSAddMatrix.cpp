#include <QClipboard>
#include "GBSAddMatrix.h"
#include "ui_GBSAddMatrix.h"
#include "gbs/GBSMainCollector.h"
#include "gbs/common/GBSHttpClient.h"
#include "gbs/dto/GBSBundleData.h"
GBSAddMatrix::GBSAddMatrix(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GBSAddMatrix)
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
	"#myEditText {"
				 "background-color: #f9f9f9;"            /* 背景色 */
				 "border: 1px solid #cccccc;"            /* 边框 */
				 "border-radius: 4px;"                  /* 圆角 */
				 "padding: 8px;"                         /* 内边距 */
				 "font-size: 14px;"                      /* 字体大小 */
				 "color: #333333;"                       /* 文本颜色 */
				 "selection-background-color: #6ec1e4;"  /* 选中文本背景 */
				 "selection-color: white;"               /* 选中文本颜色 */
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
    GBSHttpClient::getInstance()->registerHandler(this);
    connect(ui->btnClose, &QPushButton::clicked, this, [this]() { close(); });

	//ui->leActivationResult->setObjectName("myEditText");


    connect(ui->pushButton_2, &QPushButton::clicked, this, &GBSAddMatrix::activateDevice);
    ui->leActivationResult->setAlignment(Qt::AlignHCenter); 

}

GBSAddMatrix::~GBSAddMatrix()
{
    GBSHttpClient::getInstance()->unRegisterHandler(this);
    delete ui;
}

void GBSAddMatrix::activateDevice() {
	if (ui->leRemark1->text().isEmpty()) {
		ui->leActivationResult->setText("错误:备注信息没有填写");
		ui->leActivationResult->setStyleSheet(R"(
			    QLabel {
				background-color: transparent;            /* 背景色 */
				border: none;            /* 边框 */
				border-radius: 4px;                  /* 圆角 */
				font-size: 14px;                      /* 字体大小 */
				color: red;                       /* 文本颜色 */
			    }

			)");
		return;
	}
	GBSLiveAccountInfo account = GBSMainCollector::getInstance()->getAccountInfo();
	QString activationCode = ui->leActivationCode->text();
	QString deviceCode = ui->leActivationNo->text();
	int liveAccountId = account.getId();
	QString notes = ui->leRemark1->text() + "/" + ui->leRemark2->text();

	GBSHttpClient::getInstance()->addSrsLiveDeviceV2(
		activationCode.toUtf8().constData(), deviceCode.toUtf8().constData(), "Null", liveAccountId, "",
							 notes.toUtf8().constData(), "", "", "");
}

 void GBSAddMatrix::onActivateCode(int code) {
	 QMetaObject::invokeMethod(this, [code, this]() {
		 if (code == 0) {
			ui->leActivationResult->setStyleSheet(R"(
			    QLabel {
				background-color: transparent;            /* 背景色 */
				border: none;            /* 边框 */
				border-radius: 4px;                  /* 圆角 */
				font-size: 14px;                      /* 字体大小 */
				color: #00C566;                       /* 文本颜色 */
			    }

			)");
			 ui->leActivationResult->setText("激活成功");
			 
		 } else {
			ui->leActivationResult->setStyleSheet(R"(
			    QLabel {
				background-color: transparent;            /* 背景色 */
				border: none;            /* 边框 */
				border-radius: 4px;                  /* 圆角 */
				font-size: 14px;                      /* 字体大小 */
				color: red;                       /* 文本颜色 */
			    }

			)");
			 ui->leActivationResult->setText("激活失败");
		 }
		 });
	 
 }

void GBSAddMatrix::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
		event->accept();
	}
	QDialog::mousePressEvent(event);
}

void GBSAddMatrix::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton) {
		move(event->globalPosition().toPoint() - dragPosition);
		event->accept();
	}
	QDialog::mouseMoveEvent(event);
}
