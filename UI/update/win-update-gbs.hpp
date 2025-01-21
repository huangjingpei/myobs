#pragma once

#include <QThread>
#include <QString>
#include "gbs/updater/winhttp/TLWinHttpDownloader.h"

class GBSAutoUpdateThread : public QThread, public TLWinHttpDownLoaderEventHandler {
	Q_OBJECT

	bool manualUpdate;
	bool repairMode;
	bool user_confirmed = false;

	virtual void run() override;

	void info(const QString &title, const QString &text);
	int queryUpdate(bool manualUpdate, const char *text_utf8);
	bool queryRepair();


signals:
	void sigDownloadFinished();
	void sigHasNewerVersion(QList<QString> features);

private slots:
	void infoMsg(const QString &title, const QString &text);
	int queryUpdateSlot(bool manualUpdate, const QString &text);
	bool queryRepairSlot();

public:
	GBSAutoUpdateThread(bool manualUpdate_, bool repairMode_ = false)
		: manualUpdate(manualUpdate_),
		  repairMode(repairMode_)
	{
	}

private:
	bool updateConofigure{false};
	bool updateExe{false};

	// 通过 TLWinHttpDownLoaderEventHandler 继承
	void onDownloadPercentage(int32_t percentage) override;
	void onDownloadBytes(uint32_t downloadBytes, uint32_t allBytes) override;
};
