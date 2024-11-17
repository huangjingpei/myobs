#ifndef GBSBIZLIVEPUSHER_H
#define GBSBIZLIVEPUSHER_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QList>
#include <QColor>
#include <QTime>
#include "../common/DanmakuWidget.h"
#include "../common/SelectedIDSDialog.h"
#include "../GBSToolKits.h"
#include "window-basic-main-outputs.hpp"
#include "obs-frontend-api.h"
#include "gbs/common/GBSHttpClient.h"

namespace Ui {
class GBSBizLivePusher;
}

typedef struct tagDammaItem {
	QTime  currentTime;
	QString deviceName;
	QString iamgePath;
	QString danmaku;
	QString type;
} DanmaItem;

class GBSBizLivePusher : public QWidget,
	public OBSHttpEventHandler {
	Q_OBJECT

public:
	explicit GBSBizLivePusher(QWidget *parent = nullptr);
	~GBSBizLivePusher();


	void ResizePreview(uint32_t cx, uint32_t cy);
	static bool FindCameraSource(void *param, obs_source_t *source);

public:
	static void RenderMain(void *data, uint32_t cx, uint32_t cy);
	void processDanmaku();
public slots:
	// 添加新 widget 的槽函数
	//void addNewWidget();
	void onComboBoxIndexChanged(int index);
	void onStartRtmpPush();
	void StartStreaming(std::string server, std::string key);
	void StartReplayBuffer();
	//void OnEvent(enum obs_frontend_event event);

	void dialogSubmitted(const QString &data);



	QColor GetSelectionColor() const;
	QColor GetCropColor() const;
	QColor GetHoverColor() const;
	OBSScene GetCurrentScene();



public slots:
	void addNewWidget(const QString &text, const QString &imagePath, const QString &text2, const QString &type);


private slots:
	void updateStyle(bool checked);

private:
	OBSService service;
	std::unique_ptr<BasicOutputHandler> outputHandler;
	std::shared_future<void> setupStreamingGuard;
private:
	Ui::GBSBizLivePusher *ui;
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
	QString qPushUrl;
	QString qUserId;
	QString qEquipments;

	std::atomic<obs_scene_t *> currentScene = nullptr;


	int danmaFilterType = 0;// 0： 全部， 1：谁来了，2：礼物打赏，3：留言互动
	QList<QPushButton *> btnDanmaLists;


// 通过 OBSHttpEventHandler 继承
	void onLoginResult(int result) override;
	void onRtmpPushUrl(std::string url) override;
	void onPullRtmpUrl(std::string url) override;
	void onUserInfo(const GBSUserInfo *info) override;
	void onUserFileDownLoad(const std::string &path, int type) override;
	void onRoomInfos(std::list<GBSRoomInfo> &info) override;
	void onRoomInfo(GBSRoomInfo *info) override;
	void onQRcodeInfo(std::string no, std::string url, int status) override;

	QList<DanmaItem> whoIsDanmukus;
	QList<DanmaItem> allDanmakus;
	QList<DanmaItem> giftDanmakus;
	QList<DanmaItem> likeDanmakus;

};

#endif // GBSBIZLIVEPUSHER_H
