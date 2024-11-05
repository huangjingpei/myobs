#include "gbsnormalloginform.h"
#include "gbsregisterform.h"
#include "gbsmainform.h"
#include "gbsmainbizwindow.h"

#include "testUI/TestDanmakuWidget.h"
#include "testUI/TestVertNaviButton.h"
#include "testUI/TestVolumeHBarWidget.h"
#include "testUI/testToggleButton.h"
#include "testUI/testPlayerProgressBar.h"
#include <QApplication>


#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <Windows.h>
#include <QProcess>
#include <QTimer>


#include "bizWidgets/GBSBizLiveBridger.h"
#include "bizWidgets/gbsbizlivepusher.h"
#include "bizWidgets/GBSBizSettingBasic.h"
#include "naviWidgets/GBSNaviSetting.h"
#include "bizWidgets/GBSBizSettingAV.h"

#include "bizWidgets/GBSBizSettingOutput.h"

void launchChromeHidden(QProcess *&process) {
	process = new QProcess;
	QString chromePath = "C:/Program Files/Google/Chrome/Application/chrome.exe";  // Modify the path as needed
	QStringList arguments;
	arguments << "--new-window";// << "https://www.google.com";  // Open a webpage

	// Launch Chrome in a hidden state using QProcess and Windows API
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;  // Start Chrome hidden

	if (!CreateProcessW((LPCWSTR)chromePath.utf16(), nullptr, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
		qDebug() << "Failed to launch Chrome";
	} else {
		qDebug() << "Chrome started successfully, hidden.";
	}
}

void launchChrome() {
	QProcess *process = new QProcess;
	QString chromePath = "C:/Program Files/Google/Chrome/Application/chrome.exe"; // Modify the path if necessary
	QStringList arguments;
	arguments << "--new-window" << "https://www.google.com";  // Optionally open a webpage

	process->start(chromePath, arguments);

	if (!process->waitForStarted()) {
		qDebug() << "Chrome did not start.";
	} else {
		qDebug() << "Chrome started successfully.";
	}
}

HWND findChromeWindow() {
	HWND chromeHwnd = nullptr;
	chromeHwnd = FindWindowA("Chrome_WidgetWin_1", nullptr);  // The Chrome window class

	if (chromeHwnd) {
		qDebug() << "Found Chrome window.";
	} else {
		qDebug() << "Chrome window not found.";
	}

	return chromeHwnd;
}

void embedChrome(HWND chromeHwnd, QWidget *qtWidget) {
	if (chromeHwnd) {
		HWND qtHwnd = reinterpret_cast<HWND>(qtWidget->winId());
		SetParent(chromeHwnd, qtHwnd);  // Set Chrome as a child of the Qt window

		// Optionally resize/move the Chrome window to fit inside the widget
		SetWindowPos(chromeHwnd, nullptr, 0, 0, qtWidget->width(), qtWidget->height(), SWP_SHOWWINDOW);
	}
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	// 创建主窗口Test


	GBSMainBizWindow w;
	// 设置PingFang SC字体
	QFont font("PingFang SC", 10);
	w.setFont(font);
	w.show();



	return a.exec();
}


