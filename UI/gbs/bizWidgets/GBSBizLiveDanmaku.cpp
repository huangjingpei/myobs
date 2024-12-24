#include "GBSBizLiveDanmaku.h"
#include "ui_GBSBizLiveDanmaku.h"
#include "gbs/GBSMainCollector.h"
#include "gbs/common/QBizLogger.h"


#include <windows.h>
#include <tlhelp32.h>

static bool killDanmakuProcess(const QString &processName)
{
	bool killed = false;
	DWORD currentProcessId = GetCurrentProcessId();

	// Create a snapshot of all processes
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		qDebug() << "Failed to take a snapshot of processes.";
		return false;
	}

	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);

	// Loop through all processes in the snapshot
	if (Process32First(snapshot, &processEntry)) {
		do {
			QString currentProcessName = QString::fromWCharArray(processEntry.szExeFile);

			// Check if the process name matches and it's not the current instance
			if (currentProcessName.compare(processName, Qt::CaseInsensitive) == 0 &&
			    processEntry.th32ProcessID != currentProcessId) {
				HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processEntry.th32ProcessID);
				if (hProcess) {
					if (TerminateProcess(hProcess, 0)) {
						qDebug() << "Killed process:" << processName;
						killed = true;
					} else {
						qDebug() << "Failed to terminate process:" << processName;
					}
					CloseHandle(hProcess);
				}
			}
		} while (Process32Next(snapshot, &processEntry));
	}
	CloseHandle(snapshot);
	return killed;
}


GBSBizLiveDanmaku::GBSBizLiveDanmaku(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GBSBizLiveDanmaku)
{
	ui->setupUi(this);

	// ui->tabWidget->setStyleSheet(
	// "QTabWidget::pane {"
	// "    border: none;" // 移除tab pane的边框
	// "}"
	// "QTabBar::tab {"
	// "    background: none;"   // tab的背景颜色
	// "    padding: 10px;"      // tab内容的填充
	// "    margin-right: 10px;" // 调整tab之间的水平间距
	// "    border: none;"       // 移除tab的边框
	// "    color: #01C667;"
	// "    font-size:16px;"
	// "}"
	// "QTabBar::tab:first {"
	// "    margin-left: 100px;" // 调整第一个tab项的左外边距
	// "}"
	// "QTabBar::tab:selected {"
	// "    background: #C0C0C0;" // 选中tab的背景颜色
	// "    border: none;"        // 选中时也不显示边框
	// "    border-radius: 16px"
	// "}"
	// "QTabBar {"
	// "    qproperty-alignment: AlignCenter;" // 设置tab项居中对齐
	// "}");

	    ui->tabWidget->setStyleSheet("QTabWidget::pane {"
				 "    border: none;" // 移除tab pane的边框
				 "}"); // 清空 QTabWidget 的样式表

	    ui->tabWidget->tabBar()->setStyleSheet("QTabBar::tab {"
				"    color: #78828A; "              // 默认字体颜色
				"    background: none; "       // 默认背景颜色
				 "    padding: 40px; "      // 内边距
				"    padding: 4px; "             // 内边距
				"    border-radius: 5px; "        // 圆角
				"    font-size: 16px;"         // 设置字体大小为16像素

				"}"
				"QTabBar::tab:selected {"
				"    color: #00C566; "              // 选中字体颜色
				"    background: none; "       // 选中背景颜色
				"    padding: 4px; "              // 内边距
				"}"
				"QTabBar::tab:first {"
				"    margin-left: 100px;" // 调整第一个tab项的左外边距
				"}");

    ui->cbxPlat->setStyleSheet("QComboBox {"
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
				    "    width: 16px;"                                        // 箭头的宽度
				    "    height: 16px;"                                       // 箭头的高度
				    "    border: none;"
				    "    background: transparent;" // 透明背景
				    "}"
				"QComboBox QAbstractItemView {"
				"border: 1px solid #cccccc;"
				"   selection-background-color: #f0f0f0;"
				"   selection-color: black;"
				"border-radius: 0px;" /* 移除下拉列表的圆角 */

				"}"
    );
	iniFile = new IniSettings("danmu/setting/setting.ini");
	QString text = ui->leChromePath->text();
	if (text.isEmpty()) {
		QString qstr = iniFile->value("setting", "google", "").toString();
		ui->leChromePath->setText(qstr);
	}
	connect(ui->leChromePath, &QLineEdit::textChanged, this,
		[this](const QString & text) {
			iniFile->setValue("setting", "google", text);
		});


	text = ui->leServer->text();
	if (text.isEmpty()) {
		QString qstr =
			iniFile->value("host", "server", "127.0.0.1").toString();
		ui->leServer->setText(qstr);
	}
	connect(ui->leServer, &QLineEdit::textChanged, this,
		[this](const QString &text) {
			iniFile->setValue("host", "server", text);
		});


	text = ui->lePort->text();
	if (text.isEmpty()) {
		QString qstr =
			iniFile->value("host", "port", "7979")
				.toString();
		ui->lePort->setText(qstr);
	}
	connect(ui->lePort, &QLineEdit::textChanged, this,
		[this](const QString &text) {
			iniFile->setValue("host", "port", text);
		});

	text = ui->leServerType->text();
	if (text.isEmpty()) {
		QString qstr =
			iniFile->value("host", "protocol", "http")
				.toString();
		ui->leServerType->setText(qstr);
	}
	connect(ui->leServerType, &QLineEdit::textChanged, this,
		[this](const QString &text) {
			iniFile->setValue("host", "protocol", text);
		});

	text = ui->leServerType->text();
	if (text.isEmpty()) {
		QString qstr =
			iniFile->value("host", "protocol", "http").toString();
		ui->leServerType->setText(qstr);
	}
	connect(ui->leServerType, &QLineEdit::textChanged, this,
		[this](const QString &text) {
			iniFile->setValue("host", "protocol", text);
		});
	
	
	
	text = ui->leDouyin->text();
	if (text.isEmpty()) {
		QString qstr =
			iniFile->value("broadcast", "douyin", "").toString();
		ui->leDouyin->setText(qstr);
	}
	connect(ui->leDouyin, &QLineEdit::textChanged, this,
		[this](const QString &text) {
			iniFile->setValue("broadcast", "douyin", text);
		});


	text = ui->leKuaishou->text();
	if (text.isEmpty()) {
		QString qstr =
			iniFile->value("broadcast", "kuaishou", "").toString();
		ui->leKuaishou->setText(qstr);
	}
	connect(ui->leKuaishou, &QLineEdit::textChanged, this,
		[this](const QString &text) {
			iniFile->setValue("broadcast", "kuaishou", text);
		});

	text = ui->leWechatVideo->text();
	if (text.isEmpty()) {
		QString qstr =
			iniFile->value("broadcast", "shipinhao", "").toString();
		ui->leWechatVideo->setText(qstr);
	}
	connect(ui->leWechatVideo, &QLineEdit::textChanged, this,
		[this](const QString &text) {
			iniFile->setValue("broadcast", "shipinhao", text);
		});

	
	text = ui->leTikTok->text();
	if (text.isEmpty()) {
		QString qstr =
			iniFile->value("broadcast", "tiktok", "").toString();
		ui->leTikTok->setText(qstr);
	}
	connect(ui->leTikTok, &QLineEdit::textChanged, this,
		[this](const QString &text) {
			iniFile->setValue("broadcast", "tiktok", text);
		});


		
	text = ui->leTaobao->text();
	if (text.isEmpty()) {
		QString qstr =
			iniFile->value("broadcast", "taobao", "").toString();
		ui->leTaobao->setText(qstr);
	}
	connect(ui->leTaobao, &QLineEdit::textChanged, this,
		[this](const QString &text) {
			iniFile->setValue("broadcast", "taobao", text);
		});

			
	text = ui->leTaobao->text();
	if (text.isEmpty()) {
		QString qstr =
			iniFile->value("broadcast", "taobao", "").toString();
		ui->leTaobao->setText(qstr);
	}
	connect(ui->leTaobao, &QLineEdit::textChanged, this,
		[this](const QString &text) {
			iniFile->setValue("broadcast", "taobao", text);
		});

	text = ui->leMeiTuan->text();
	if (text.isEmpty()) {
		QString qstr =
			iniFile->value("broadcast", "meituan", "").toString();
		ui->leMeiTuan->setText(qstr);
	}
	connect(ui->leMeiTuan, &QLineEdit::textChanged, this,
		[this](const QString &text) {
			iniFile->setValue("broadcast", "meituan", text);
		});

	text = ui->lePddLive->text();
	if (text.isEmpty()) {
		QString qstr =
			iniFile->value("broadcast", "pdd", "").toString();
		ui->lePddLive->setText(qstr);
	}
	connect(ui->lePddLive, &QLineEdit::textChanged, this,
		[this](const QString &text) {
			iniFile->setValue("broadcast", "pdd", text);
		});

	text = ui->leOther->text();
	if (text.isEmpty()) {
		QString qstr =
			iniFile->value("broadcast", "other", "").toString();
		ui->leOther->setText(qstr);
	}
	connect(ui->leOther, &QLineEdit::textChanged, this,
		[this](const QString &text) {
			iniFile->setValue("broadcast", "other", text);
		});

	QList<QString> livePlats = GBSMainCollector::getInstance() -> getLivePlats();
	ui->cbxPlat->addItems(livePlats);
	QString cbxplatString = iniFile->value("broadcast", "plat", "douyin").toString();
	if (cbxplatString == "douyin") {
		ui->cbxPlat->setCurrentText(livePlats[0]);
	} else if (cbxplatString == "kuaishou")
	{
		ui->cbxPlat->setCurrentText(livePlats[1]);
	} else if (cbxplatString == "shipinhao")
	{
		ui->cbxPlat->setCurrentText(livePlats[3]);
	} else if (cbxplatString == "TikTok")
	{
		ui->cbxPlat->setCurrentText(livePlats[5]);
	} else if (cbxplatString == "淘宝")
	{
	} else if (cbxplatString == "bili")
	{
		ui->cbxPlat->setCurrentText(livePlats[2]);
	} else if (cbxplatString == "pdd")
	{
		ui->cbxPlat->setCurrentText(livePlats[4]);
	} else if (cbxplatString == "facebook")
	{
		ui->cbxPlat->setCurrentText(livePlats[6]);
	}

	connect(ui->cbxPlat, &QComboBox::activated, this,
		[this](int index) {
			QString plat = ui->cbxPlat->currentText();
			if (plat == "抖音") {
				iniFile->setValue("broadcast", "plat", "douyin");
			} else if (plat == "快手") {
				iniFile->setValue("broadcast", "plat",
						  "kuaishou");

			} else if (plat == "视频号") {
				iniFile->setValue("broadcast", "plat",
						  "shipinhao");
			} else if (plat == "TikTok") {
				iniFile->setValue("broadcast", "plat",
						  "tiktok");
			} else if (plat == "淘宝") {
			} else if (plat == "哔哩哔哩") {
				iniFile->setValue("broadcast", "plat",
						  "bili");
			} else if (plat == "拼多多") {
				iniFile->setValue("broadcast", "plat", "pdd");
			} else if (plat == "Facebook") {
				iniFile->setValue("broadcast", "plat", "facebook");
			}

		});



	
	ui->pushButton_15->setStyleSheet("QPushButton {"
					"background-color: #00C566;"
					"border-radius:5px;"
					"color: white;"
					 "font-size: 16px;"
					"}"
				);
	ui->pushButton_15->setText("关闭");
	ui->pushButton_15->setCheckable(true);
	connect(ui->pushButton_15, &QPushButton::clicked, this, [this]() {

		QPushButton *button = qobject_cast<QPushButton *>(sender());
		if (!danmakuRunning) {
			
			QProcess *chromeProcess = nullptr;
			launchDanmuExe(chromeProcess);
			HWND chromeHwnd = nullptr;

			//// Wait for a few seconds for Chrome to launch and get the window handle
			//QTimer::singleShot(2000, [&]() {
			//	chromeHwnd = findChromeWindow();
			//	if (chromeHwnd) {
			//		embedChrome(
			//			chromeHwnd,
			//			ui->wgtEmbedChrome); // Embed Chrome into the main window
			//	}
			//});

			QList<QString> liveAbbreviations = GBSMainCollector::getInstance()->getLiveAbbreviations();
			QString liveAbbreviation = liveAbbreviations.at(ui->cbxPlat->currentIndex());
			GBSMainCollector::getInstance()->setDanmakuPlat(liveAbbreviation.toStdString());
			QLogD("Start danmaku crawling");
			ui->pushButton_15->setText("开启");
			ui->pushButton_15->setStyleSheet("QPushButton {"
							 "background-color: #D7D7D7;" 
							 "border-radius:5px;"
							 "color: white;"
							 "font-size: 16px;"
							 "}");
		} else {
			ui->pushButton_15->setText("关闭");
			ui->pushButton_15->setStyleSheet("QPushButton {"
							 "background-color: #00C566;"
							 "border-radius:5px;"
							 "color: white;"
							 "font-size: 16px;"
							 "}");
			QLogD("Stop danmaku crawling");
			GBSMainCollector::getInstance()->setDanmakuPlat("");
			QString procceessName = "main.exe";
			killDanmakuProcess(procceessName);

		}
		danmakuRunning = !danmakuRunning;
		});

}

HWND GBSBizLiveDanmaku::findChromeWindow()
{
	HWND chromeHwnd = nullptr;
	chromeHwnd = FindWindowA("Chrome_WidgetWin_1",
				 nullptr); // The Chrome window class

	if (chromeHwnd) {
		qDebug() << "Found Chrome window.";
	} else {
		qDebug() << "Chrome window not found.";
	}

	return chromeHwnd;
}

GBSBizLiveDanmaku::~GBSBizLiveDanmaku()
{
	if (iniFile) {
		delete iniFile;
	}
	delete ui;
}


void GBSBizLiveDanmaku::launchDanmuExe(QProcess *&process)
{
	//process = new QProcess;
	//QString danmu =
	//	QCoreApplication::applicationDirPath() + "/danmu/main.exe";
	////process->start(danmu);

	////    if (!process->waitForStarted()) {
	////	qDebug() << "启动失败:" << process->errorString();
	////} else {
	////	qDebug() << "程序已启动";
	////}

	//// Launch Chrome in a hidden state using QProcess and Windows API
	//STARTUPINFO si = {sizeof(si)};
	//PROCESS_INFORMATION pi;
	//si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_NORMAL; //SW_HIDE; // Start Chrome hidden

	//if (!CreateProcessW((LPCWSTR)danmu.utf16(), nullptr, nullptr,
	//		    nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
	//	qDebug() << "Failed to launch Chrome";
	//} else {
	//	qDebug() << "Chrome started successfully, hidden.";
	//}


	QString danmu = QCoreApplication::applicationDirPath() + "/danmu/main.exe";
	// 可执行文件路径
	LPCWSTR applicationName = reinterpret_cast<LPCWSTR>(danmu.utf16());

	// 设置进程信息
	PROCESS_INFORMATION processInfo;
	ZeroMemory(&processInfo, sizeof(processInfo));

	// 设置启动信息
	STARTUPINFO startupInfo;
	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	startupInfo.dwFlags = STARTF_USESHOWWINDOW;
	startupInfo.wShowWindow = SW_HIDE; // SW_HIDE 可用于隐藏窗口

	// 创建进程
	if (CreateProcess(applicationName, NULL, NULL, NULL, FALSE, 0,
				NULL, NULL, &startupInfo, &processInfo)) {
		qDebug() << "程序已启动";
		//// 等待进程结束
		//WaitForSingleObject(processInfo.hProcess, INFINITE);

		//// 关闭句柄
		//CloseHandle(processInfo.hProcess);
		//CloseHandle(processInfo.hThread);
	} else {
		qDebug() << "启动失败: " << GetLastError();
				
	}


}

void GBSBizLiveDanmaku::onMessage(std::string msg) {}

void GBSBizLiveDanmaku::onOpen() {}

void GBSBizLiveDanmaku::onClose() {}



void GBSBizLiveDanmaku::embedChrome(HWND chromeHwnd, QWidget *qtWidget)
{
	if (chromeHwnd) {
		HWND qtHwnd = reinterpret_cast<HWND>(qtWidget->winId());
		SetParent(chromeHwnd,
			  qtHwnd); // Set Chrome as a child of the Qt window

		// Optionally resize/move the Chrome window to fit inside the widget
		SetWindowPos(chromeHwnd, nullptr, 0, 0, qtWidget->width(),
			     qtWidget->height(), SWP_SHOWWINDOW);
	}
}
