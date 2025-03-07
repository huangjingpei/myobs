#ifndef GBSBIZLIVEGUARDERCTRL_H
#define GBSBIZLIVEGUARDERCTRL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QTimer>
#include <QPointer>
#include <QThreadStorage>
#include <QMap>
#include <QPushButton>
#include "window-basic-main-outputs.hpp"

#include "gbs/common/GBSHttpClient.h"
#include "gbs/dto/GBSLiveDevices.h"
#include "gbs/common/WebSocketClient.h"
#include "gbs/bizWidgets/danmaku/DanmakuWidget.h"
#include "gbs/bizWidgets/danmaku/DanmakuBizWindow.h"

#include "gbs/GBSDanmaType.h"
namespace Ui {
class GBSBizLiveGuarderCtrl;
}
class GridButtons;
class LiveManageWidget;
class GBSBizLiveGuarderCtrl : public QWidget, public OBSHttpEventHandler, public WssEventListener {
    Q_OBJECT

public:
	explicit GBSBizLiveGuarderCtrl(QWidget *parent = nullptr);
	~GBSBizLiveGuarderCtrl();

	void onListDevices(std::list<GBSLiveDevices> devices, int pageNum) override;
	void onDeviceCount(int onLineCount, int totalCount) override;


signals:

	void signalDanmakuReceived(const QString &text, const QString &imagePath, const QString &text2,
				   const QString &type, const QString &liveId);

public slots:
	void onTabChanged(int index);
	void onTabChanged2(int index);

	void addNewWidget(const QString &atext, const QString &aimagePath, const QString &atext2, const QString &atype,
			  const QString &liveId);
	void onWssKeepAlive();

	void onDanmukuChanged(int value);

	

protected:
	static void RenderMain(void *data, uint32_t cx, uint32_t cy);
	void ResizePreview(uint32_t cx, uint32_t cy);

	void processDanmaItem(const nlohmann::json jsonObject);

private:
	std::unique_ptr<BasicOutputHandler> outputHandler;
	std::shared_future<void> setupStreamingGuard;

private:
	// 通过 WssEventListener 继承
	void onMessage(std::string msg) override;
	void onOpen() override;
	void onFail() override;
	void onClose() override;

	void onPullRtmpUrl(const std::string url);
	void updateStyle(bool checked);


private:
    Ui::GBSBizLiveGuarderCtrl *ui;

private:
	QWidget *validWidget = nullptr;

	int previewX = 0, previewY = 0;
	int previewCX = 0, previewCY = 0;
	float previewScale = 0.0f;
	GridButtons *gridButtons;
	LiveManageWidget *liveManageWidget;
	std::list<GBSLiveDevices> currentliveDevices;
	int currentPageNum{0};
	QString danmaPlatIconString;
	QPointer<QTimer> mWssTimer;
	std::atomic_bool mWssRunning{false};
	std::string mWssKeepaliveId{""};
	std::shared_ptr<WebSocketClient> mWebSocketClient;


	QString pullRtmpUrl;

	int deviceOnLineCount{0};
	int deviceTotalCount{0};
	int userDanmakuType{DANITEM_TYPE_ALL};
	int userLastDanmakuType{DANITEM_TYPE_ALL};
	QMap<std::string, int> operationMap;
	QList<QPushButton *> btnDanmaLists;

	DanmakuBizWindow *danmaBizWindow;


};

#endif // GBSBIZLIVEGUARDERCTRL_H
