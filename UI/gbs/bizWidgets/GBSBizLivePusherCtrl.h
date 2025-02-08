#ifndef GBSBIZLIVEPUSHERCTRL_H
#define GBSBIZLIVEPUSHERCTRL_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QList>
#include <QColor>
#include <QTimer>
#include <QTime>
#include <QPointer>
#include <QThreadStorage>
#include "../common/DanmakuWidget.h"
#include "../common/SelectedIDSDialog.h"
#include "../GBSToolKits.h"
#include "window-basic-main-outputs.hpp"
#include "obs-frontend-api.h"
#include "gbs/common/GBSHttpClient.h"
#include "gbs/GBSDanmaType.h"
#include "gbs/common/WebSocketClient.h"
#include "gbs/media/GBSAudioWriter.h"


namespace Ui {
class GBSBizLivePusherCtrl;
}


class GBSBizLivePusherCtrl : public QWidget, public OBSHttpEventHandler, public WssEventListener {
	Q_OBJECT

public:
	explicit GBSBizLivePusherCtrl(QWidget *parent = nullptr);
	~GBSBizLivePusherCtrl();


	void ResizePreview(uint32_t cx, uint32_t cy);
	static bool FindCameraSource(void *param, obs_source_t *source);

public:
	static void RenderMain(void *data, uint32_t cx, uint32_t cy);
public slots:
	// 添加新 widget 的槽函数
	//void addNewWidget();
	void onComboBoxIndexChanged(int index);
	
	void onStartWork(bool checked);
	void StartStreaming(std::string server, std::string key);
	void StopStreaming();
	void StartReplayBuffer();
	//void OnEvent(enum obs_frontend_event event);

	void dialogSubmitted(const QString &data);

	void streamCheck();



	QColor GetSelectionColor() const;
	QColor GetCropColor() const;
	QColor GetHoverColor() const;
	OBSScene GetCurrentScene();

	
signals:
	void signalDanmakuReceived(const QString &text, const QString &imagePath, const QString &text2,
				   const QString &type);

public slots:
	void addNewWidget(const QString &text, const QString &imagePath, const QString &text2, const QString &type);

	void slotStartLiveTranscribe();


private slots:
	void updateStyle(bool checked);
	void onTimeout();
	void onTabChanged(int index);
	void onTabChanged2(int index);
	void onWssKeepAlive();


private:
	OBSService service;
	std::unique_ptr<BasicOutputHandler> outputHandler;
	std::shared_future<void> setupStreamingGuard;
private:
	Ui::GBSBizLivePusherCtrl *ui;
	QScrollArea *danmakuscrollArea;

	QVBoxLayout* danmaKuAreaLayout;

	SelectedIDSDialog *selectIDSDialog;
	QList<DanmakuWidget*> widgetList;
	int danmaindex;

public:
	int previewX = 0, previewY = 0;
	int previewCX = 0, previewCY = 0;
	float previewScale = 0.0f;
	int currentCameraIndex = 0;
	GBSSource *cameraSource = nullptr;


private:
	// 通过 WssEventListener 继承
	void onMessage(std::string msg) override;
	void onOpen() override;
	void onFail() override;
	void onClose() override;


	void processDanmaItem(const nlohmann::json jsonObject);

	void startPushStream(std::string url, int liveAccountId);

	void GetLiveTranscribeStatus();

private:
	QString qPushUrl;
	QString qUserId;
	QString qEquipments;

	std::atomic<obs_scene_t *> currentScene = nullptr;


	int danmaFilterType = 0;// 0： 全部， 1：谁来了，2：礼物打赏，3：留言互动
	QList<QPushButton *> btnDanmaLists;


// 通过 OBSHttpEventHandler 继承
	void onLoginResult(int result) override;
	void onPullRtmpUrl(std::string url) override;
	void onPushRtmpClosed() override;
	void onUserInfo(const GBSUserInfo *info) override;
	void onUserFileDownLoad(const std::string &path, int type) override;
	void onRoomInfos(std::list<GBSRoomInfo> &info) override;
	void onRoomInfo(GBSRoomInfo *info) override;
	void onQRcodeInfo(std::string no, std::string url, int status) override;
	void onRtmpPushUrl(std::string url, int liveAccountId) override;
	void onPushStreamInfo(GBSPushStreamInfo result) override;
	void onRtmpPushError(std::string errMsg) override;
	void onListDevices(std::list<GBSLiveDevices> devices, int pageNum) override;
	QList<DanmaItem> whoIsDanmukus;
	QList<DanmaItem> allDanmakus;
	QList<DanmaItem> giftDanmakus;
	QList<DanmaItem> likeDanmakus;

	QThreadStorage<QList<DanmaItem>> thlWhoIsDanmukus;
	QThreadStorage<QList<DanmaItem>> thlAllDanmakus;
	QThreadStorage<QList<DanmaItem>> thlGiftDanmakus;
	QThreadStorage<QList<DanmaItem>> thlLikeDanmakus;
	QThreadStorage<QList<DanmaItem>> thlChatDanmakus;

private:
	bool startLive {false};
	QTimer *mHeartBeatTimer;
	int mLiveAccountId = {0};
	QString danmaPlatIconString;
	QPointer<QTimer> mWssTimer;
	std::atomic_bool mWssRunning{false};
	std::string mWssKeepaliveId{""};
	std::shared_ptr<WebSocketClient> mWebSocketClient;
	std::shared_ptr<GBSAudioWriter> audioWriter;
	std::mutex mDanmakuListMtx;
	int mDanmakuType{DANITEM_TYPE_ALL};

	std::atomic<bool> onFailedProcessing{false};
	std::atomic<bool> startWorking{false};
};

#endif // GBSBIZLIVEPUSHERCTRL_H
