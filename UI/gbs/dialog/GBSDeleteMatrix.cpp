#include <QClipboard>
#include "GBSAddBroker.h"
#include "ui_GBSAddBroker.h"
#include "gbs/GBSMainCollector.h"
#include "gbs/common/GBSHttpClient.h"
#include "gbs/dto/GBSBundleData.h"
GBSAddBroker::GBSAddBroker(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GBSAddBroker)
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

        ui->btnCancel->setStyleSheet("QPushButton:pressed {"
			     "   padding-left: 3px;" // 向左移动 3px
			     "   padding-top: 3px;"  // 向上移动 3px
			     "   background-repeat: no-repeat;"
			     "   background-position: center;"
			     "}");
    ui->btnOK->setStyleSheet("QPushButton:pressed {"
			     "   padding-left: 3px;"         // 向左移动 3px
			     "   padding-top: 3px;"          // 向上移动 3px
			     "   background-repeat: no-repeat;"
			     "   background-position: center;"
			     "}"
    );

	ui->comboBox->setStyleSheet("QComboBox {"
				"    padding-left: 10px;" // 为左侧箭头和文本之间留出足够的空间
				"    border: 1px solid transparent;" // 去除边框
				"    background: transparent;"       // 背景透明

				"}"
				"QComboBox::drop-down {"
				"    width: 20px;" // 设置下拉箭头的宽度
				"    subcontrol-origin: padding;"
				"    subcontrol-position: left center;" // 将下拉按钮移到左侧
				"    margin-left: 5px;"                 // 控制箭头与左边框的距离
				"}"
				"QComboBox::down-arrow {"
				"    image: url(:gbs/images/gbs/biz/gbs-right-combox-arrow.png);" // 替换为你的箭头图标
				"    width: 16px;"                                                // 箭头的宽度
				"    height: 16px;"                                               // 箭头的高度
				"    border: none;"
				"    background: transparent;" // 透明背景
				"}"
				"QComboBox QAbstractItemView {"
				"border: 1px solid #cccccc;"
				"   selection-background-color: #f0f0f0;"
				"   selection-color: black;"
				"border-radius: 0px;" /* 移除下拉列表的圆角 */

				"}");

	QList<QString> plats = GBSMainCollector::getInstance()->getLivePlats();
    ui->comboBox->addItems(plats);
    ui->btnOK->setObjectName("myButton");
    ui->btnCancel->setObjectName("myButton");

    QClipboard *clipboard = QApplication::clipboard();
    QString text = clipboard->text();
    bool isValidContent = true;
    if (!text.isEmpty()) {
	    GBSBundleData bundleData = GBSBundleData::deserialize(text.toStdString());
	    if ((bundleData.getActivateId() == "") && (bundleData.getDeviceID() == "") &&
		(bundleData.getProductID() == "") && (bundleData.getUniqueIID() == "")) {
		    isValidContent = false;
	    }
	    if (isValidContent) {
		    GBSLiveAccountInfo account = GBSMainCollector::getInstance()->getAccountInfo();
		    QString deviceCode = QString::fromLocal8Bit(bundleData.getActivateId()); //激活编号
		    QString deviceId = QString::fromLocal8Bit(bundleData.getDeviceID());     //设备ID
		    QString productId = QString::fromLocal8Bit(bundleData.getProductID());   //产品ID
		    QString uniqueId = QString::fromLocal8Bit(bundleData.getUniqueIID());    //主板ID

		    ui->leActivationNo->setText(deviceCode);
		    ui->leDeviceNo->setText(deviceId);
		    ui->leProductNo->setText(productId);
		    ui->leUniqueID->setText(uniqueId);
	    }
    }


    connect(ui->btnClose, &QPushButton::clicked, this, [this]() { close();
	    });

    connect(ui->btnCancel, &QPushButton::clicked, this, [this]() { reject();
	    });

    connect(ui->btnOK, &QPushButton::clicked, this, [this]() {
	    QClipboard *clipboard = QApplication::clipboard();
	    QString text = clipboard->text();

	    GBSLiveAccountInfo account = GBSMainCollector::getInstance()->getAccountInfo();
	    QString activationCode = ui->leActivationCode->text();
	    QString deviceCode = ui->leActivationNo->text();
	    QString deviceName = ui->leDeviceName->text();
	    int liveAccountId = account.getId();
	    QString deviceNo = ui->leDeviceNo->text();
	    QString productNo = ui->leProductNo->text();
	    QString notes = ui->leRemark1->text() + "/" + ui->leRemark2->text();
	    QString platformAccount = ui->lePlatformAccount->text();
	    QString toDeskAccount = ui->leRemoteAccount->text();
	    QString toDeskPassword = ui->leRemotePassword->text();
	    QString livePlatform = ui->comboBox->currentText();
	    QString leUniqueId = ui->leUniqueID->text();


	    GBSHttpClient::getInstance()->activateDeviceV2(
			activationCode.toUtf8().constData(), deviceName.toUtf8().constData(),
			deviceNo.toUtf8().constData(), liveAccountId, livePlatform.toUtf8().constData(),
			leUniqueId.toUtf8().constData(),
			notes.toUtf8().constData(), 
			platformAccount.toUtf8().constData(),
			productNo.toUtf8().constData(),
			toDeskAccount.toUtf8().constData(), toDeskPassword.toUtf8().constData());

	    GBSHttpClient::getInstance()->addSrsLiveDeviceV2(activationCode.toUtf8().constData(),
						deviceCode.toUtf8().constData(),
						deviceName.toUtf8().constData(),
						liveAccountId,
						livePlatform.toUtf8().constData(),
						notes.toUtf8().constData(),
						platformAccount.toUtf8().constData(), 
						toDeskAccount.toUtf8().constData(),
						toDeskPassword.toUtf8().constData());


	    accept();

		GBSHttpClient::getInstance()->pageSrsLiveDeviceV2(account.getId(), 0);
	    });
    

}

GBSAddBroker::~GBSAddBroker()
{
    delete ui;
}


void GBSAddBroker::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
		event->accept();
	}
	QDialog::mousePressEvent(event);
}

void GBSAddBroker::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton) {
		move(event->globalPosition().toPoint() - dragPosition);
		event->accept();
	}
	QDialog::mouseMoveEvent(event);
}
