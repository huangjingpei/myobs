#include "GBSMainProfile.h"
#include "ui_GBSMainProfile.h"
#include "common/VertNaviButton.h"
#include <QMouseEvent>
#include <QEvent>
#include <QSpacerItem>
#include "gbs/GBSMainBizWindow.h"
#include "gbs/GBSNormalLoginForm.h"
#include "window-basic-main.hpp"
#include "window-basic-settings.hpp"
#include "gbs/bizWidgets/GBSMsgDialog.h"
#include "gbs/GBSMainCollector.h"
#include "gbs/dto/GBSLiveAccountInfo.h"
#include <QDialog>


GBSMainProfile::GBSMainProfile(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GBSMainProfile)
{

    setSizePolicy(
        QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint |
                   Qt::WindowMaximizeButtonHint |
                   Qt::WindowCloseButtonHint);
    ui->setupUi(this);

    VertNaviButton* btnAccount = new VertNaviButton("账号资料", ":gbs/images/gbs/biz/gbs-menu-profile.png", this);
    VertNaviButton* btnAboutME = new VertNaviButton("关于我们", ":gbs/images/gbs/biz/gbs-menu-about-me.png", this);
    VertNaviButton* btnSoftWare = new VertNaviButton("软件更新", ":gbs/images/gbs/biz/gbs-menu-software-update.png", this);
    VertNaviButton* btnSetting = new VertNaviButton("高级设置", ":gbs/images/gbs/biz/gbs-menu-setting.png", this);
    VertNaviButton* btnHotkey = new VertNaviButton("快捷键", ":gbs/images/gbs/biz/gbs-menu-hotkey.png", this);
    VertNaviButton* btnAPI = new VertNaviButton("API接口", ":gbs/images/gbs/biz/gbs-menu-api.png", this);
    VertNaviButton* btnMaterial = new VertNaviButton("素材背景库", ":gbs/images/gbs/biz/gbs-menu-material.png", this);
    VertNaviButton* btnDriver = new VertNaviButton("摄像头驱动", ":gbs/images/gbs/biz/gbs-menu-driver.png", this);
    VertNaviButton* btnSquare = new VertNaviButton("音色广场", ":gbs/images/gbs/biz/gbs-menu-timbre-square.png", this);
    vertNaviButtons << btnAccount << btnAboutME << btnSoftWare << btnSetting << btnHotkey << btnAPI << btnMaterial
		    << btnDriver << btnSquare;
    for (auto vertNaviButton : vertNaviButtons) {
	    connect(vertNaviButton, &VertNaviButton::clicked, this,
		    [this, vertNaviButton](bool checked) {
		    VertNaviButton *button = qobject_cast<VertNaviButton *>(sender());
			for (int i = 0; i < vertNaviButtons.count(); i++) {
				VertNaviButton *it = vertNaviButtons.at(i);
				if (it == button) {
					it->changeStyle(true);
				} else {
					it->changeStyle(false);
				}
			}
	
	    });
    }
	//QString btnStyle = R"(
	//	QPushButton {
	//		background-color:#00C566;
	//		border: 1px solid white;
	//		border-radius:10px;
	//		font-size:20px;
	//	}

	//	QPushButton:pressed {
	//		background-color: #D1D8DD;
	//		padding-left: 3px;"       
	//		padding-top: 3px;"        
	//		background-repeat: no-repeat;
	//		background-position: center;
	//	}
	//	QPushButton:hover {
	//	   background-color: #F9F9F9;
	//	}
	//)";

	//btnSoftWare->setStyleSheet(btnStyle);

    btnAccount->setFixedSize(300, 40);
    btnAboutME->setFixedSize(300, 40);
    btnSoftWare->setFixedSize(300, 40);
    btnSetting->setFixedSize(300, 40);
    btnHotkey->setFixedSize(300, 40);
    btnAPI->setFixedSize(300, 40);
    btnMaterial->setFixedSize(300, 40);
    btnDriver->setFixedSize(300, 40);
    btnSquare->setFixedSize(300, 40);

    ui->verticalLayout->addWidget(btnAccount);
    ui->verticalLayout->addWidget(btnAboutME);
    ui->verticalLayout->addWidget(btnSoftWare);
    ui->verticalLayout->addWidget(btnSetting);
    ui->verticalLayout->addWidget(btnHotkey);
    ui->verticalLayout->addWidget(btnAPI);
    ui->verticalLayout->addWidget(btnMaterial);
    ui->verticalLayout->addWidget(btnDriver);
    ui->verticalLayout->addWidget(btnSquare);
    QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayout->addSpacerItem(verticalSpacer);


    ui->pushButton_2->setStyleSheet(
        "QPushButton {"
        "   background-color: #8F92A1;"
        "   border: #000000;" // 无边框
        "   border-radius: 10px;" // 圆角
        "   font-size: 14px;"
        "   text-align: center;" // 左对齐文本
        "}"

        "QPushButton:hover {"
        "   background-color: #1B2846;"
        "   color: white;"  // 悬停状态字体颜色

        "}"
        "QPushButton:pressed {"
        "   background-color: #F9F9F9;" // 按下状态背景颜色
        "   padding-left: 1px;  /* 向左移动 3px */"
        "   padding-top: 1px;    /* 向上移动 3px */"
        "   color: white;"  // 按下状态字体颜色
        "}"
        );
    //QString appDirPath = QCoreApplication::applicationDirPath();
    //appDirPath += "/avator.png";
    //QString style = "background-image: url(./avator.png);";
    //ui->label_3->setStyleSheet(style);

    QString dialogStyleSheet = R"(
				"QDialog { "
				  "    background-color: grey; " // 背景色
				  "    border: 2px solid #000000; " // 边框
				  "    border-radius: 10px; "       // 圆角
				  "} "
				  "QLabel { "
				  "    color: white; "    // 文本颜色
				  "    font-size: 16px; " // 字体大小
				  "} "
				  "QPushButton { "
				  "    background-color: #2E3A59; "
				  "    color: white; "
				  "    border: 1px solid #4C4C4C; "
				  "    border-radius: 5px; "
				  "    padding: 5px 10px; "
				  "} "
				  "QPushButton:hover { "
				  "    background-color: #3B4A72; "
				  "} "
				  "QComboBox { "
				  "    background-color: #2E3A59; "
				  "    color: white; "
				  "    border: 1px solid #4C4C4C; "
				  "    border-radius: 5px; "
				  "    padding: 2px 5px; "
				  "} "
				  "QComboBox::drop-down { "
				  "    border: 0px; "
				  "} "
				  "QComboBox::down-arrow { "
				  "    image: url(:/icons/down-arrow.png); "
				  "    width: 10px; height: 10px; "
				  "} "
				  "QCheckBox, QRadioButton { "
				  "    color: white; "
				  "    font-size: 14px; "
				  "} "
				  "QGroupBox { "
				  "    color: white; "              // 标题颜色
				  "    font-size: 16px; "           // 标题字体大小
				  "    border: 1px solid #4C4C4C; " // 边框
				  "    border-radius: 5px; "        // 圆角
				  "    margin-top: 10px; "          // 标题上方的间距
				  "} "
				  "QGroupBox:title { "
				  "    subcontrol-origin: margin; "
				  //"    subcontrol-position: top center; " // 标题居中
				  "    padding: 0 5px; "
				  "    background-color: #2E3A59; "
				  "    border-radius: 5px; "
				  "}"
			)";


    OBSBasic *main = OBSBasic::Get();
    QString path = main->getAvator();
    if (!path.isEmpty()) {
	    QPixmap pixmap(path);
	    ui->label_3->setPixmap(pixmap.scaled(48, 48, Qt::KeepAspectRatio));
    }
    GBSLiveAccountInfo account = GBSMainCollector::getInstance()->getAccountInfo();

    QString nickName = QString::fromStdString(account.getNickname());
    ui->label_2->setText(nickName);
    
    // ui->label_3->setStyleSheet(" border-radius: 50%;");
    connect(ui->pushButton_2,  &QPushButton::clicked, this, &GBSMainProfile::exitSystemAndGoLogin);
    connect(btnSetting, &QPushButton::clicked, this, [dialogStyleSheet, main]() {
	    QVBoxLayout *layout = new QVBoxLayout();
	    OBSBasicSettings settings(main);
	    QWidget *widget = settings.getAdvancedPageWidget();
	    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	    layout->addWidget(widget);
	    layout->addWidget(buttonBox);

	    QDialog *dialog = new QDialog();
	    dialog->setStyleSheet(dialogStyleSheet);
	    dialog->setLayout(layout);
	    // 连接按钮信号
	    connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept); // 确认按钮
	    connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject); // 取消按钮
	    dialog->exec();
	    });


    
    connect(btnHotkey, &QPushButton::clicked, this, [dialogStyleSheet, main]() {
	    QVBoxLayout *layout = new QVBoxLayout();
	    OBSBasicSettings settings(main);
	    QWidget *widget = settings.getHotkeyPageWidget();
	    widget->setFixedSize(453, 815);
	    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	    layout->addWidget(widget);
	    layout->addWidget(buttonBox);

	    QDialog *dialog = new QDialog();
	    dialog->setStyleSheet(dialogStyleSheet);
	    dialog->setLayout(layout);
	    // 连接按钮信号
	    connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept); // 确认按钮
	    connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject); // 取消按钮
	    dialog->exec();
	    });

    connect(btnSoftWare, &QPushButton::clicked, this, [main]() { main->checkGBSForUpdate(true);
	    });

    connect(btnAboutME, &QPushButton::clicked, this, [this]() {
	    QWidget *widget = new QWidget;
	    QVBoxLayout *layout = new QVBoxLayout(widget);

	    layout->setAlignment(Qt::AlignHCenter); // 整体内容居中
	    QSpacerItem *spacer0 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Minimum);
	    layout->addSpacerItem(spacer0);
	    QLabel *swVersion = new QLabel();
	    QString qVersion = "软件版本号：";
	    qVersion += GBSMainCollector::getInstance()->getSoftWareVersion();
	    swVersion->setStyleSheet("font-size: 20px;");
	    swVersion->setText(qVersion);
	    QLabel *swBuildTime = new QLabel();
	    QString qBuildTime = "软件编译时间：";
	    swBuildTime->setStyleSheet("font-size: 20px;");
	    qBuildTime += GBSMainCollector::getInstance()->getBuildInfo();
	    swBuildTime->setText(qBuildTime);
	    QLabel *swCopyright = new QLabel();
	    swCopyright->setStyleSheet("font-size: 20px;");
	    swCopyright->setText("软件版权：杭州国播技术有限公司");
	    

	    layout->addWidget(swVersion);
	    layout->addWidget(swBuildTime);
	    layout->addWidget(swCopyright);

	    QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	    layout->addSpacerItem(spacer);
	    GBSMsgDialog *dialog = new GBSMsgDialog("关于我们", layout, this);
	    dialog->exec();
	    });
    timer = new QTimer(this);
    timer->setInterval(1000); // 1秒无点击重置计数器
    connect(ui->pushButton, &QPushButton::clicked, this, &GBSMainProfile::onShowSetting);
    connect(timer, &QTimer::timeout, this, &GBSMainProfile::resetClickCount);
}

void GBSMainProfile::exitSystemAndGoLogin(bool cheked) {
    
	QMetaObject::invokeMethod(App()->GetMainWindow(), "close");
	
    QTimer::singleShot(2000, [this]() {
	    GBSNormalLoginForm *loginForm = new GBSNormalLoginForm(this);
	    loginForm->show();
    });

}

void GBSMainProfile::onShowSetting()
{
	clickCount++;
	if (!timer->isActive()) {
		timer->start(); 
	}
	if (clickCount >= 5) {
		clickCount = 0; 
		timer->stop();
		OBSBasic *main = OBSBasic::Get();
		main->on_action_Settings_triggered();

	} else {

	}
}

void GBSMainProfile::resetClickCount()
{
	clickCount = 0;
}


GBSMainProfile::~GBSMainProfile()
{
    delete ui;
}


