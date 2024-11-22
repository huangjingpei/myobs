#ifndef GBSLIVEDANMAKU_H
#define GBSLIVEDANMAKU_H

#include <QWidget>
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <Windows.h>
#include <QProcess>
#include <QTimer>

#include "window-basic-main-outputs.hpp"
#include "obs-frontend-api.h"


#include "gbs/common/QIniFile.h"
#include "gbs/common/WebSocketClient.h"

namespace Ui {
class GBSBizLiveDanmaku;
}

class GBSBizLiveDanmaku : public QWidget, public WssEventListener {
	Q_OBJECT

public:
	explicit GBSBizLiveDanmaku(QWidget *parent = nullptr);
	~GBSBizLiveDanmaku();

private:
	HWND  findChromeWindow();
	void embedChrome(HWND chromeHwnd, QWidget *qtWidget);
	void launchDanmuExe(QProcess *&process);

private:
	Ui::GBSBizLiveDanmaku *ui;

	IniSettings *iniFile;


	// 通过 WssEventListener 继承
	void onMessage(std::string msg) override;
	void onOpen() override;
	void onClose() override;
};

#endif // GBSLIVEDANMAKU_H
