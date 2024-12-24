#include "GBSActivateDevice.h"
#include "ui_GBSActivateDevice.h"
#include "gbs/common/GBSHttpClient.h"
#include "gbs/GBSMainCollector.h"
#include <QClipboard>

GBSActivateDevice::GBSActivateDevice(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GBSActivateDevice)
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
    ui->btnOK->setStyleSheet("QPushButton {"
				 "background-color:#00C566"
				 "border: 1px solid white"
				 "border-radius:10px"
				 "font-size:20px"
			     "}"
				"QPushButton:pressed {"
				"   padding-left: 3px;" // 向左移动 3px
				"   padding-top: 3px;"  // 向上移动 3px
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
				"   font-size: 24px;"
				"border-radius: 0px;" /* 移除下拉列表的圆角 */

				"}");




    connect(ui->pushButton, &QPushButton::clicked, this, [this]() {
	    QString text = ui->leActivateCode->text();
	    copyToClipboard(text);
	    });
    ui->btnOK->setObjectName("myButton");
    ui->btnCancel->setObjectName("myButton");
    QList<QString> platLists = GBSMainCollector::getInstance()->getLivePlats();
    QList<QString> platListLatin = GBSMainCollector::getInstance()->getLiveAbbreviations();

    ui->comboBox->addItems(platLists);
    connect(ui->btnClose, &QPushButton::clicked, this, [this]() { close();
	    });

    connect(ui->btnCancel, &QPushButton::clicked, this, [this]() { reject();
	    });

    connect(ui->btnOK, &QPushButton::clicked, this, [this]() {
	    

            connect(ui->comboBox, &QComboBox::currentIndexChanged, this, [](int index) {

		    });
	    QString livePlatform = ui->comboBox->currentText();
	    QString notes = ui->leCustom1->text() + "/" + ui->leCustom2->text();
	    int mLiveAccoutnId = ui->leLiveAccount->text().toInt();
	    QString toDeskAccount = ui->leRemoteAccount->text();
	    QString toDeskPassword = ui->leRemotePassword->text();
	    QString deviceName = ui->leDeviceName->text();
	    GBSHttpClient::getInstance()->activateDeviceV2(
		    "123456", deviceName.toUtf8().constData(),
							    mLiveAccoutnId,
							    livePlatform.toUtf8().constData(),
							    notes.toUtf8().constData(), livePlatform.toUtf8().constData(),
							    toDeskAccount.toUtf8().constData(),
							    toDeskPassword.toUtf8().constData()
							    );
	    accept();
	    });


}

GBSActivateDevice::~GBSActivateDevice()
{
    delete ui;
}

void GBSActivateDevice::copyToClipboard(const QString text)
{
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(text); // 将文本复制到剪贴板
}


void GBSActivateDevice::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
		event->accept();
	}
	QDialog::mousePressEvent(event);
}

void GBSActivateDevice::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton) {
		move(event->globalPosition().toPoint() - dragPosition);
		event->accept();
	}
	QDialog::mouseMoveEvent(event);
}


void GBSActivateDevice::addCustomerNo(std::string customNo)
{
	ui->leDeviceName->setText(QString::fromStdString(customNo));
}

void GBSActivateDevice::addLiveAccountId(int liveAccountId)
{
	ui->leLiveAccount->setText(QString::number(liveAccountId));
}

void GBSActivateDevice::addActivateCode(std::string activateCode) {
	ui->leActivateCode->setText(QString::fromStdString(activateCode));
}
void GBSActivateDevice::addPlatformAccount(std::string platformAccount)
{
	ui->comboBox->setCurrentText(QString::fromStdString(platformAccount));
}

void GBSActivateDevice::addRemark1(std::string mark) {
	ui->leCustom1->setText(QString::fromStdString(mark));
}
void GBSActivateDevice::addRemark2(std::string mark) {
	ui->leCustom2->setText(QString::fromStdString(mark));
}
void GBSActivateDevice::addRemoteUsername(std::string username) {	
	ui->leRemoteAccount->setText(QString::fromStdString(username));

}
void GBSActivateDevice::addRemotePassword(std::string password) {
	ui->leRemotePassword->setText(QString::fromStdString(password));
}
void GBSActivateDevice::addRemoteSwitcher(int value) {
	ui->horizontalSlider->setValue(value);
}

void GBSActivateDevice::diableActivate() {
	ui->btnOK->setDisabled(true);
	ui->btnOK->setStyleSheet(R"(background-color:grey;
		border: 1px solid white;
		border-radius:10px;
		font-size:20px;
		)");
}
