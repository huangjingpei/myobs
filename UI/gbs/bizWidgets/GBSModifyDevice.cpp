#include <QClipboard>
#include "GBSModifyDevice.h"
#include "ui_GBSModifyDevice.h"
#include "gbs/GBSMainCollector.h"
#include "gbs/common/GBSHttpClient.h"
#include "gbs/dto/GBSBundleData.h"
GBSModifyDevice::GBSModifyDevice(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GBSModifyDevice)
{
    ui->setupUi(this);
     setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    //setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

     setStyleSheet(R"(
    
    QLineEdit:disabled, QPushButton:disabled {
        background-color: #f2f2f2;  /* 禁用时背景灰色 */
        color: #777777;             /* 禁用时字体颜色为灰色 */
        border: 2px solid #cccccc;  /* 边框颜色 */
        border-radius: 5px;         /* 圆角 */
        font-style: italic;         /* 可选：使用斜体字体 */
    }
	)");

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



    connect(ui->btnClose, &QPushButton::clicked, this, [this]() { close();
	    });

    connect(ui->btnCancel, &QPushButton::clicked, this, [this]() { reject();
	    });

    connect(ui->btnOK, &QPushButton::clicked, this, [this]() {
	    if (liveDeviceName.empty()) {
		    liveDeviceName = ui->leDeviceName->text().toStdString();
	    }
	    if (liveAccountId.empty()) {
		    liveAccountId = ui->lePlatformAccount->text().toStdString();
	    }
	    if (remoteAccountId.empty()) {
		    remoteAccountId = ui->leRemoteAccount->text().toStdString();
	    }
	    if (remotePassword.empty()) {
		    remotePassword = ui->leRemotePassword->text().toStdString();
	    }
	    if (remark.empty() && remark1.empty()) {
		    remark = ui->leRemark1->text().toStdString();
		    remark1 = ui->leRemark2->text().toStdString();
	    }
	    GBSHttpClient::getInstance()->modifyZlmLiveDevice(liveDeviceName, liveDeviceId, livePlat,
							      remark + "/" + remark1, liveAccountId, remoteSwitch, remoteAccountId, remotePassword);
	    accept();

	    });

}

GBSModifyDevice::~GBSModifyDevice()
{
    delete ui;
}


void GBSModifyDevice::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
		event->accept();
	}
	QDialog::mousePressEvent(event);
}

void GBSModifyDevice::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton) {
		move(event->globalPosition().toPoint() - dragPosition);
		event->accept();
	}
	QDialog::mouseMoveEvent(event);
}



void GBSModifyDevice::setLiveDeviceId(int id) {
	ui->leLiveDeviceID->setText(QString::number(id, 10));
	ui->leLiveDeviceID->setReadOnly(true);
	ui->leLiveDeviceID->setDisabled(true);

	liveDeviceId = id;
}

void GBSModifyDevice::setLiveDeviceName(std::string name, bool select) 
{
	if (select != true) {
		ui->leDeviceName->setText(QString::fromLocal8Bit(name));
		ui->leDeviceName->setReadOnly(true);
		liveDeviceName = name;
		ui->leDeviceName->setDisabled(true);


	} 
}
std::string GBSModifyDevice::getLiveDeviceName() {

	return liveDeviceName;
}

void GBSModifyDevice::setLiveAccountId(std::string id, bool select) {
	if (select != true) {
		ui->lePlatformAccount->setText(QString::fromLocal8Bit(id));
		ui->lePlatformAccount->setReadOnly(true);
		liveAccountId = id;
		ui->lePlatformAccount->setDisabled(true);

	} 
	
}
std::string GBSModifyDevice::getLiveAccountId() {
	return liveAccountId;
}

void GBSModifyDevice::setRemoteAccountId(std::string id, bool select) {
	if (!select) {
		ui->leRemoteAccount->setText(QString::fromLocal8Bit(id));
		ui->leRemoteAccount->setReadOnly(true);
		remoteAccountId = id;
		ui->leRemoteAccount->setDisabled(true);

	}


}
std::string GBSModifyDevice::getRemoteAccountId() {
	return remoteAccountId;
}

void GBSModifyDevice::setRemotePassword(std::string id, bool select)
{
	if (!select) {
		ui->leRemotePassword->setText(QString::fromLocal8Bit(id));
		ui->leRemotePassword->setReadOnly(true);
		remotePassword = id;
		ui->leRemotePassword->setDisabled(true);


	}
}
std::string GBSModifyDevice::getRemotePassword() {
	return remotePassword;
}

void GBSModifyDevice::setRemarks(std::string r, std::string r1, bool select)
{
	remark = r;
	remark1 = r1;
	if (!select) {
		ui->leRemark1->setText(QString::fromLocal8Bit(r));
		ui->leRemark2->setText(QString::fromLocal8Bit(r1));
		ui->leRemark1->setReadOnly(true);
		ui->leRemark2->setReadOnly(true);
		ui->leRemark1->setDisabled(true);
		ui->leRemark2->setDisabled(true);

	}

}
std::string GBSModifyDevice::getRemarks() {
	return remark + "/" + remark1;
}

void GBSModifyDevice::setRemoteSwitch(bool enable, bool select) {
	remoteSwitch = enable;
	if (!select) {
		ui->horizontalSlider->setValue(remoteSwitch == true ? 1 : 0);
		ui->horizontalSlider->setDisabled(true);
	}
	
}
bool GBSModifyDevice::getRemoteSwitch() {
	return remoteSwitch;
}

void GBSModifyDevice::setLivePlat(std::string plat, int index, bool select)
{
	if (!select) {
		ui->comboBox->setDisabled(true);
		QStringList mangedItems = GBSMainCollector::getInstance()->getLiveAbbreviations();
		ui->comboBox->addItems(mangedItems);

	} 
}
std::string GBSModifyDevice::getLivePlat() {
	return livePlat;
}
