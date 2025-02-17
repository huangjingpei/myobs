#include <string>
#include <algorithm>
#include <vector>
#include <cinttypes>
#include <memory>
#include <QScopedPointer>
#include <QTimer>
#include <QFile>
#include <QPainterPath>
#include "window-basic-main.hpp"
#include "update/win-update-gbs.hpp"

#include <qt-wrappers.hpp>
#include <window-basic-main-outputs.hpp>
#include "gbs/common/FatButton.h"
#include "gbs/common/QIniFile.h"
#include "gbs/common/GBSHttpClient.h"
#include "gbs/common/QBizLogger.h"
#include "gbs/naviWidgets/GBSNaviData.h"
#include "gbs/common/WebSocketClient.h"
#include "gbs/GBSMainCollector.h"
#include "gbs/common/SystemUtils.h"

#include "gbs/GBSAsioHttpServer.h"
#include <QScrollBar>
#include <QTimer>
#include "gbs/bizWidgets/GBSMsgDialog.h"

#include <iostream>
#include <chrono>
#include <thread>


// 本窗体是为了初始化自己窗口的函数，区别于OBSInit的是: 后者是初始化OBS系统，前者只是简单的初始化一个窗口
// 这里你也可以理解为OBSBasic 及负责了整个除登录页面生命周期意外的所有软件的生命周期

/*Http server*/
// std::shared_ptr<GBSAsioHttpServer> gHttpServer;

struct AddSourceData2 {
	/* Input data */
	obs_source_t *source;
	bool visible;
	obs_transform_info *transform = nullptr;
	obs_sceneitem_crop *crop = nullptr;
	obs_blending_method *blend_method = nullptr;
	obs_blending_type *blend_mode = nullptr;

	/* Return data */
	obs_sceneitem_t *scene_item = nullptr;
};


static void AddSource2(void *_data, obs_scene_t *scene)
{
	AddSourceData2 *data = (AddSourceData2 *)_data;
	obs_sceneitem_t *sceneitem;

	sceneitem = obs_scene_add(scene, data->source);

	if (data->transform != nullptr)
		obs_sceneitem_set_info2(sceneitem, data->transform);
	if (data->crop != nullptr)
		obs_sceneitem_set_crop(sceneitem, data->crop);
	if (data->blend_method != nullptr)
		obs_sceneitem_set_blending_method(sceneitem, *data->blend_method);
	if (data->blend_mode != nullptr)
		obs_sceneitem_set_blending_mode(sceneitem, *data->blend_mode);

	obs_sceneitem_set_visible(sceneitem, data->visible);

	data->scene_item = sceneitem;
}

static bool remove_items2(obs_scene_t *, obs_sceneitem_t *item, void *param)
{
	std::vector<OBSSceneItem> &items = *reinterpret_cast<std::vector<OBSSceneItem> *>(param);

	if (obs_sceneitem_selected(item)) {
		items.emplace_back(item);
	} else if (obs_sceneitem_is_group(item)) {
		obs_sceneitem_group_enum_items(item, remove_items2, &items);
	}
	return true;
};


void OBSBasic::OBSInit2() {



	ui->pushButton->setImgAndTxt(":gbs/images/gbs/biz/gbs-live-console-camera.png",
					":gbs/images/gbs/biz/gbs-live-console-camera-color.png", "摄像头驱动");
	ui->pushButton_2->setImgAndTxt(":gbs/images/gbs/biz/gbs-live-console-record.png",
					":gbs/images/gbs/biz/gbs-live-console-record-color.png", "录制画面");
	ui->pushButton_3->setImgAndTxt(":gbs/images/gbs/biz/gbs-live-console-rmdup.png",
					":gbs/images/gbs/biz/gbs-live-console-rmdup-color.png", "源直播去重");
	ui->pushButton_4->setImgAndTxt(":gbs/images/gbs/biz/gbs-live-console-ai-splite-speech.png",
					":gbs/images/gbs/biz/gbs-live-console-ai-splite-speech-color.png", "大型话术拆分");
	ui->pushButton_5->setImgAndTxt(":gbs/images/gbs/biz/gbs-live-console-translate.png",
					":gbs/images/gbs/biz/gbs-live-console-translate-color.png", "开启直播翻译");
	ui->pushButton_6->setImgAndTxt(":gbs/images/gbs/biz/gbs-live-console-danmaku-interaction.png",
					":gbs/images/gbs/biz/gbs-live-console-danmaku-interaction-color.png", "弹幕回复");

	GBSHttpClient::getInstance()->registerHandler(this);

	

	
	
	connect(ui->pushButton_2, &FatButton::clicked, this, &OBSBasic::RecordActionTriggered);
	connect(ui->pushButton, &FatButton::clicked, this, &OBSBasic::VirtualCamActionTriggered);

	ui->actionMixerToolbarMenu->setVisible(false);

	cleanGuarderCtrlScene();

	mAudioReader = std::make_unique<GBSAudioReader>();
	mAudioReader->SetAudioReaderCallback(this);
	mAudioReader->Initialize();
	const std::lock_guard<std::mutex> lock(mRtcEngineMutex);
	mRtcEngine = ZegoRTCEngine::Create();
	mRtcEngine->CreateEngine();

	GBSHttpClient::getInstance()->srsLiveAccountInfoV2("多多客");

	//gHttpServer = GBSAsioHttpServer::Create();
	//gHttpServer->start();

	mWssTimer = new QTimer(this);
	connect(mWssTimer, &QTimer::timeout, this, &OBSBasic::onWssKeepAlive);
	if (!mWssTimer->isActive()) {
		mWssTimer->start();
		mWssTimer->setInterval(10000);
	}

}


void OBSBasic::OBSDeinit2() {
	if (mWssTimer->isActive()) {
		mWssTimer->stop();
	}
	QLogD("OBSDeinit2, End pull task.");
	if (mWssClient) {
		mWssClient->UnRegisterHandler(this);
		mWssClient->Stop();
	}
	GBSHttpClient::getInstance()->unRegisterHandler(this);
	endPullTask();
	const std::lock_guard<std::mutex> lock(mRtcEngineMutex);

	if (mRtcEngine != nullptr) {
		mRtcEngine->DestroyEngine();
		mRtcEngine = nullptr;
	}

	if (mWssClient != nullptr) {
		mWssClient->Stop();
	}

	// if (gHttpServer != nullptr) {
	// 	gHttpServer->stop();
	// }
	// 在应用程序退出前调用
	QLog::Logger::instance().shutdown();
}

void OBSBasic::onAudioCapture(void *data, int size, uint64_t ts) {
	const std::lock_guard<std::mutex> lock(mRtcEngineMutex);
	if (mRtcEngine) {
		mRtcEngine->PushAudioData(data, size, ts);
	}
}

void OBSBasic::beginTalk(int id)
{
	const std::lock_guard<std::mutex> lock(mRtcEngineMutex);
	if (mRtcEngine) {
		GBSLiveAccountInfo accountInfo = GBSMainCollector::getInstance()->getAccountInfo();
		std::string roomId = std::to_string(accountInfo.getUserId()) + "_" + std::to_string(id);
		//mRtcEngine->LoginAndStartPush(roomId, std::to_string(id), std::to_string(id));
		
		mRtcEngine->LoginRoom(roomId, std::to_string(accountInfo.getLiveDeviceId()));
		mRtcEngine->BeginTalk(std::to_string(accountInfo.getLiveDeviceId()), nullptr);
	}
}

void OBSBasic::endTalk(int id)
{
	const std::lock_guard<std::mutex> lock(mRtcEngineMutex);
	if (mRtcEngine) {
		//mRtcEngine->LogoutAndStopPush();
		mRtcEngine->EndTalk();
		//mRtcEngine->LogoutRoom();
	}
}

void OBSBasic::onMessage(std::string msg){
	bool needProcess = !GBSMainCollector::getInstance()->isLiving();
	if (needProcess) {
		QMetaObject::invokeMethod(this, [msg, this]() {
			QLogD("Receive message from PullStreamNotifier %s", msg.c_str());
			if (msg == "接到刷新getPullStreamUrl接口通知") {
				QLogD("收到开始拉流，客户端开始拉流");
				beginPullTask();
			} else if (msg == "接到拉流端停止推流通知") {
				QLogD("收到停止拉流，客户端停止拉流");
				endPullTask();
				stopPullStream();
			}
			});
	}
}
void OBSBasic::onOpen(){
	mWssRunning = true;
}

void OBSBasic::onFail(){
	mWssRunning = false;
	Sleep(5 * 1000);
	QMetaObject::invokeMethod(this, [this]() {
		mWssClient->UnRegisterHandler(this);
		mWssClient->Stop();
		mWssClient = WebSocketClient::Create();
		mWssClient->RegisterHandler(this);
		mWssClient->setName("PullStreamNotifier");
		std::string url = GBSMainCollector::getInstance()->getBaseWebSocketV2();
		std::string wssUrl = url + "/refreshPullStream/" + mWssKeepaliveId;
		mWssClient->Start(wssUrl);
		});

}

void OBSBasic::onWssKeepAlive() {
	if (mWssClient && mWssRunning) {
		std::string target = "ping_" + mWssKeepaliveId;
		mWssClient->Send(target);
		dumpFFmegSourceLog();

	}
}

void OBSBasic::onClose()
{
	mWssRunning = false;
}


QString OBSBasic::GetVendor()
{
	static QString vendor = "";
	std::unique_ptr<IniSettings> iniFile = std::make_unique<IniSettings>("gbs.ini");
	if (vendor.isEmpty()) {
		vendor = iniFile->value("System", "vendor", "").toString();
		if (vendor.isEmpty()) {
			vendor = "GBS";
		}
	}
	return vendor;
}

void OBSBasic::beginPullTask() {
	QLogD("Begin pull task.");
	//if (pullStreamTimer != nullptr) {
	//	return;
	//}
	//pullStreamTimer = new QTimer(this);
	//connect(pullStreamTimer.data(), &QTimer::timeout, this,
	//	[this]() {
			GBSHttpClient::getInstance()->getPullStreamUrlV2();
			QPoint center = this->rect().center(); // 获取窗口的矩形区域的中心点
			
		//});
	
	//pullStreamTimer->start(5000);
}
void OBSBasic::endPullTask() {
	QLogD("End pull task.");
	//if (pullStreamTimer != nullptr) {
	//	pullStreamTimer->stop();
	//	delete pullStreamTimer;
	//	pullStreamTimer = nullptr;
	//}
}

void OBSBasic::onPullRtmpUrl(const std::string url) {
	qDebug() << "onPullRtmpUrl " << url;
	
	if (!url.empty()) {
		QString newUrl = QString::fromStdString(url);
		if (newUrl.startsWith("rtmp://",Qt::CaseInsensitive) == 0) {
			stopPullStream();
			return;
		}
		if (pullRtmpUrl != newUrl) {
			pullRtmpUrl = newUrl;
			QMetaObject::invokeMethod(this, "startPullStream", Q_ARG(QString, pullRtmpUrl));
		}
		obs_source_t *source = obs_get_source_by_name("RTMP 矩阵地址");
		if (source) {
			obs_source_set_enabled(source, true);
			return;
		}
	}
}
//void OBSBasic::stopPullStream() {
//	size_t idx = 0;
//	const char *unversioned_type;
//	const char *type;
//	OBSSource newSource;
//	while (obs_enum_input_types2(idx++, &type, &unversioned_type)) {
//		const char *name = obs_source_get_display_name(type);
//		uint32_t caps = obs_get_source_output_flags(type);
//
//		if (strcmp(type, "ffmpeg_source") == 0) {
//			//AddSource(unversioned_type);
//			//AddNew
//			OBSSourceAutoRelease source = obs_get_source_by_name("RTMP 矩阵地址");
//			if (source) {
//				OBSSceneItem newSceneItem;
//				const char *v_id = obs_get_latest_input_type_id("ffmpeg_source");
//				OBSScene scene = querySceneBySceneName("场景");
//				if (source && scene) {
//					obs_source_set_audio_active(source, false); // 停止音频
//					obs_source_remove(source);
//					obs_source_release(source);
//				}
//			}
//		}
//	}
//}

void OBSBasic::stopPullStream()
{
	#if 0
	obs_source_t *source = obs_get_source_by_name("RTMP 矩阵地址");
	if (source) {
+
	}
	#else
	obs_source_t *source = obs_get_source_by_name("RTMP 矩阵地址");
	if (source) {
		obs_source_set_enabled(source, false);
	}
	
	#endif
}

void OBSBasic::dumpFFmegSourceLog () {
	//size_t idx = 0;
	//const char *unversioned_type;
	//const char *type;
	//OBSSource newSource;
	//while (obs_enum_input_types2(idx++, &type, &unversioned_type)) {
	
	//	const char *name = obs_source_get_display_name(type);
	//	uint32_t caps = obs_get_source_output_flags(type);

	//	if (strcmp(type, "ffmpeg_source") == 0) {
	//		//AddSource(unversioned_type);
	//		//AddNew
	//		OBSSourceAutoRelease source = obs_get_source_by_name("RTMP 矩阵地址");
	//		if (source) {
	//			OBSSceneItem newSceneItem;
	//			const char *v_id = obs_get_latest_input_type_id("ffmpeg_source");
	//			OBSScene scene = querySceneBySceneName("场景");
	//			if (source && scene) {
	//				obs_data_t *settings = obs_source_get_settings(source);
	//				obs_data_set_bool(settings, "log_changes", true);
	//				obs_source_update(source, settings);
	//				obs_data_release(settings);
	//				
	//			}
	//		}
	//	}
	//}
}


void OBSBasic::startPullStream(QString rtmp)
{
	size_t idx = 0;
	const char *unversioned_type;
	const char *type;



	std::unique_ptr<IniSettings> iniFile = std::make_unique<IniSettings>("gbs.ini");
	iniFile->setValue("LiveBroker", "url", pullRtmpUrl);
	while (obs_enum_input_types2(idx++, &type, &unversioned_type)) {
		const char *name = obs_source_get_display_name(type);
		uint32_t caps = obs_get_source_output_flags(type);

		if (strcmp(type, "ffmpeg_source") == 0) {
			//AddSource(unversioned_type);
			//AddNew
			OBSSourceAutoRelease source = obs_get_source_by_name("RTMP 矩阵地址");
			if (source) {
				obs_properties_t *props = obs_source_properties(source);
				obs_property_t *property = obs_properties_first(props);
				obs_property_t *is_local_file = obs_properties_get(props, "is_local_file");
				if (is_local_file) {
					obs_data_t *settings = obs_source_get_settings(source);
					obs_data_set_bool(settings, "is_local_file", false);
					obs_source_update(source, settings);
					obs_data_release(settings);
				}
				obs_property_t *input = obs_properties_get(props, "input");
				if (input) {
					obs_data_t *settings = obs_source_get_settings(source);
					obs_data_set_string(settings, "input", rtmp.toStdString().c_str());
					obs_source_update(source, settings);
					obs_data_release(settings);
				}
			} else {

				OBSSceneItem newSceneItem;
				OBSSource newSource;
				const char *v_id = obs_get_latest_input_type_id("ffmpeg_source");
				source = obs_source_create(v_id, "RTMP 矩阵地址", NULL, nullptr);
				OBSScene scene = GetCurrentScene();
				if (source && scene) {
					AddSourceData2 data;
					data.source = source;
					data.visible = true;

					obs_enter_graphics();
					obs_scene_atomic_update(scene, AddSource2, &data);
					obs_leave_graphics();

					newSource = source;
					newSceneItem = data.scene_item;

					/* set monitoring if source monitors by default */
					uint32_t flags = obs_source_get_output_flags(source);
					if ((flags & OBS_SOURCE_MONITOR_BY_DEFAULT) != 0) {
						obs_source_set_monitoring_type(source,
									       OBS_MONITORING_TYPE_MONITOR_ONLY);
					}

					bool closed = true;
					if (properties)
						closed = properties->close();

					obs_properties_t *props = obs_source_properties(newSource);
					obs_property_t *property = obs_properties_first(props);
					obs_property_t *is_local_file = obs_properties_get(props, "is_local_file");
					if (is_local_file) {
						obs_data_t *settings = obs_source_get_settings(source);
						obs_data_set_bool(settings, "is_local_file", false);
						obs_source_update(source, settings);
						obs_data_release(settings);
					}

					obs_property_t *input = obs_properties_get(props, "input");
					if (input) {
						obs_data_t *settings = obs_source_get_settings(source);
						obs_data_set_string(settings, "input", rtmp.toStdString().c_str());
						obs_source_update(source, settings);
						obs_data_release(settings);
					}

					if (!closed) {
						properties = new OBSBasicProperties(this, newSource);
						properties->Init();
						properties->setAttribute(Qt::WA_DeleteOnClose, true);
					}
				}
			}
		}
	}
	
}

QString OBSBasic::getAvator() {
	QString file = QCoreApplication::applicationDirPath() + "/round-avator.png";
	QFile qFile(file);
	if (qFile.exists()) {
		return file;
	} else {
		return QCoreApplication::applicationDirPath() + "/avator.png";
	}
}


void OBSBasic::onUserInfo(const GBSUserInfo *info) {
	if (!info->GetHead().empty()) {
		GBSHttpClient::getInstance()->downFile(info->GetHead(), "avator.png", 0);
	}
	
}

void OBSBasic::onRoomInfos(std::list<GBSRoomInfo> &info) {}

void OBSBasic::onRoomInfo(GBSRoomInfo *info) {}
void OBSBasic::onAccountInfo(GBSLiveAccountInfo result){

	GBSMainCollector::getInstance()->setAccountInfo(result);
	const std::lock_guard<std::mutex> lock(mRtcEngineMutex);
	if (mRtcEngine && !mRtcEngine->IsLogin()) {
		std::string userId = GBSMainCollector::getInstance()->getLiveDeviceId();
		mRtcEngine->setScenario(true, false, userId, nullptr);
		std::string roomId = std::to_string(result.getUserId()) + "_" + std::to_string(result.getLiveDeviceId());
		mRtcEngine->LoginRoom(roomId, userId);
		
	}

	if (mWssClient == nullptr) {
		std::string url = GBSMainCollector::getInstance()->getBaseWebSocketV2();
		std::string uniqueId = GBSMainCollector::getInstance()->getSystemUniqueNo();
		mWssClient = WebSocketClient::Create();
		mWssClient->setName("PullStreamNotifier");
		mWssClient->RegisterHandler(this);
		std::string id = std::to_string(result.getId()) + "_" + GetMachineIdFromRegistry() + GetWindowsProductIDFromRegistery() + uniqueId;
		mWssKeepaliveId = id;
		std::string wssUrl = url + "/refreshPullStream/" + id;
		mWssClient->Start(wssUrl);
	}
	
}


OBSSource OBSBasic::addCameraSource()
{
	size_t idx = 0;
	const char *unversioned_type;
	const char *type;
	OBSSource newSource;
	while (obs_enum_input_types2(idx++, &type, &unversioned_type)) {
		const char *name = obs_source_get_display_name(type);
		uint32_t caps = obs_get_source_output_flags(type);

		if (strcmp(type, "dshow_input") == 0) {
			//AddSource(unversioned_type);
			//AddNew
			OBSSourceAutoRelease source = obs_get_source_by_name("视频采集设备");
			if (source) {

			} else {

				OBSSceneItem newSceneItem;
				const char *v_id = obs_get_latest_input_type_id("dshow_input");
				source = obs_source_create(v_id, "视频采集设备", NULL, nullptr);
				OBSScene scene = GetCurrentScene();
				if (source && scene) {
					AddSourceData2 data;
					data.source = source;
					data.visible = true;

					obs_enter_graphics();
					obs_scene_atomic_update(scene, AddSource2, &data);
					obs_leave_graphics();

					newSource = source;
					newSceneItem = data.scene_item;

					/* set monitoring if source monitors by default */
					uint32_t flags = obs_source_get_output_flags(source);
					if ((flags & OBS_SOURCE_MONITOR_BY_DEFAULT) != 0) {
						obs_source_set_monitoring_type(source,
									       OBS_MONITORING_TYPE_MONITOR_ONLY);
					}

					bool closed = true;
					if (properties)
						closed = properties->close();


					obs_properties_t *props = obs_source_properties(newSource);
					obs_property_t *property = obs_properties_first(props);
					obs_property_t *is_local_file = obs_properties_get(props, "is_local_file");
					obs_property_t *prop_device = obs_properties_get(props, "video_device_id");
					if (prop_device) {
						size_t count = obs_property_list_item_count(prop_device);
						for (size_t i = 0; i < count; ++i) {
							if (i == 0) {
								const char *device_name =
									obs_property_list_item_name(prop_device, i);
								const char *device_id =
									obs_property_list_item_string(prop_device, i);

								// 选择合适的设备
								obs_data_t *settings = obs_source_get_settings(source);
								obs_data_set_string(settings, "video_device_id",
										    device_id);
								obs_source_update(source, settings);
								obs_data_release(settings);
							}
						}
					}

					if (!closed) {
						properties = new OBSBasicProperties(this, newSource);
						properties->Init();
						properties->setAttribute(Qt::WA_DeleteOnClose, true);
					}
				}
			}
		}
	}
	return newSource;
}


OBSSource OBSBasic::addMicrophoneSource() {
	size_t idx = 0;
	const char *unversioned_type;
	const char *type;
	OBSSource newSource;
	while (obs_enum_input_types2(idx++, &type, &unversioned_type)) {
		const char *name = obs_source_get_display_name(type);
		uint32_t caps = obs_get_source_output_flags(type);

		if (strcmp(type, "wasapi_input_capture") == 0) {
			//AddSource(unversioned_type);
			//AddNew
			OBSSourceAutoRelease source = obs_get_source_by_name("场控对讲设备");
			if (source) {

			} else {

				OBSSceneItem newSceneItem;
				const char *v_id = obs_get_latest_input_type_id("wasapi_input_capture");
				source = obs_source_create(v_id, "场控对讲设备", NULL, nullptr);
				OBSScene scene = GetCurrentScene();
				if (source && scene) {
					AddSourceData2 data;
					data.source = source;
					data.visible = true;

					obs_enter_graphics();
					obs_scene_atomic_update(scene, AddSource2, &data);
					obs_leave_graphics();

					newSource = source;
					newSceneItem = data.scene_item;

					/* set monitoring if source monitors by default */
					uint32_t flags = obs_source_get_output_flags(source);
					if ((flags & OBS_SOURCE_MONITOR_BY_DEFAULT) != 0) {
						obs_source_set_monitoring_type(source,
									       OBS_MONITORING_TYPE_MONITOR_ONLY);
					}

					bool closed = true;
					if (properties)
						closed = properties->close();

					obs_properties_t *props = obs_source_properties(newSource);
					obs_property_t *property = obs_properties_first(props);
					obs_property_t *prop_device = obs_properties_get(props, "device_id");
					if (prop_device) {
						size_t count = obs_property_list_item_count(prop_device);
						for (size_t i = 0; i < count; ++i) {
							if (i == 0) {
								const char *device_name =
									obs_property_list_item_name(prop_device, i);
								const char *device_id =
									obs_property_list_item_string(prop_device, i);

								// 选择合适的设备
								obs_data_t *settings = obs_source_get_settings(source);
								obs_data_set_string(settings, "device_id",
										    device_id);
								obs_source_update(source, settings);
								obs_data_release(settings);
							}
						}
					}

					if (!closed) {
						properties = new OBSBasicProperties(this, newSource);
						properties->Init();
						properties->setAttribute(Qt::WA_DeleteOnClose, true);
					}
				}
			}
		}
	}
	return newSource;
}


OBSSource OBSBasic::addSpeakerSource()
{
	size_t idx = 0;
	const char *unversioned_type;
	const char *type;
	OBSSource newSource;
	while (obs_enum_input_types2(idx++, &type, &unversioned_type)) {
		const char *name = obs_source_get_display_name(type);
		uint32_t caps = obs_get_source_output_flags(type);

		if (strcmp(type, "wasapi_output_capture") == 0) {
			//AddSource(unversioned_type);
			//AddNew
			OBSSourceAutoRelease source = obs_get_source_by_name("场控对讲输出设备");
			if (source) {

			} else {

				OBSSceneItem newSceneItem;
				const char *v_id = obs_get_latest_input_type_id("wasapi_output_capture");
				source = obs_source_create(v_id, "场控对讲输出设备", NULL, nullptr);
				OBSScene scene = GetCurrentScene();
				if (source && scene) {
					AddSourceData2 data;
					data.source = source;
					data.visible = true;

					obs_enter_graphics();
					obs_scene_atomic_update(scene, AddSource2, &data);
					obs_leave_graphics();

					newSource = source;
					newSceneItem = data.scene_item;

					/* set monitoring if source monitors by default */
					uint32_t flags = obs_source_get_output_flags(source);
					if ((flags & OBS_SOURCE_MONITOR_BY_DEFAULT) != 0) {
						obs_source_set_monitoring_type(source,
									       OBS_MONITORING_TYPE_MONITOR_ONLY);
					}

					bool closed = true;
					if (properties)
						closed = properties->close();

					obs_properties_t *props = obs_source_properties(newSource);
					obs_property_t *property = obs_properties_first(props);
					obs_property_t *prop_device = obs_properties_get(props, "device_id");
					if (prop_device) {
						size_t count = obs_property_list_item_count(prop_device);
						for (size_t i = 0; i < count; ++i) {
							if (i == 0) {
								const char *device_name =
									obs_property_list_item_name(prop_device, i);
								const char *device_id =
									obs_property_list_item_string(prop_device, i);

								// 选择合适的设备
								obs_data_t *settings = obs_source_get_settings(source);
								obs_data_set_string(settings, "device_id", device_id);
								obs_source_update(source, settings);
								obs_data_release(settings);
							}
						}
					}

					if (!closed) {
						properties = new OBSBasicProperties(this, newSource);
						properties->Init();
						properties->setAttribute(Qt::WA_DeleteOnClose, true);
					}
				}
			}
		}
	}
	return newSource;
}


OBSSource OBSBasic::addPCMAudioSource()
{
	size_t idx = 0;
	const char *unversioned_type;
	const char *type;
	OBSSource newSource;
	while (obs_enum_input_types2(idx++, &type, &unversioned_type)) {
		const char *name = obs_source_get_display_name(type);
		uint32_t caps = obs_get_source_output_flags(type);

		if (strcmp(type, "pcm_audio_source") == 0) {
			//AddSource(unversioned_type);
			//AddNew
			OBSSourceAutoRelease source = obs_get_source_by_name("场控对讲输出设备");
			if (source) {
				newSource = source;
			} else {

				OBSSceneItem newSceneItem;
				const char *v_id = obs_get_latest_input_type_id("pcm_audio_source");
				source = obs_source_create(v_id, "场控对讲输出设备", NULL, nullptr);
				OBSScene scene = GetCurrentScene();
				if (source && scene) {
					AddSourceData2 data;
					data.source = source;
					data.visible = true;

					obs_enter_graphics();
					obs_scene_atomic_update(scene, AddSource2, &data);
					obs_leave_graphics();

					newSource = source;
					newSceneItem = data.scene_item;

					/* set monitoring if source monitors by default */
					uint32_t flags = obs_source_get_output_flags(source);
					if ((flags & OBS_SOURCE_MONITOR_BY_DEFAULT) != 0) {
						obs_source_set_monitoring_type(source,
									       OBS_MONITORING_TYPE_MONITOR_ONLY);
					}

					bool closed = true;
					if (properties)
						closed = properties->close();


					if (!closed) {
						properties = new OBSBasicProperties(this, newSource);
						properties->Init();
						properties->setAttribute(Qt::WA_DeleteOnClose, true);
					}
				}
			}
		}
	}
	return newSource;
}

void OBSBasic::removeMicrophoneSource() {
	std::vector<OBSSceneItem> items;
	OBSScene scene = GetCurrentScene();
	obs_source_t *scene_source = obs_scene_get_source(scene);

	obs_scene_enum_items(scene, remove_items2, &items);
	if (!items.size())
		return;

	
	/* ----------------------------------------------- */
	/* save undo data                                  */

	OBSData undo_data = BackupScene(scene_source);

	/* ----------------------------------------------- */
	/* remove items                                    */

	for (auto &item : items)
		obs_sceneitem_remove(item);

	/* ----------------------------------------------- */
	/* save redo data                                  */

	OBSData redo_data = BackupScene(scene_source);

	/* ----------------------------------------------- */
	/* add undo/redo action                            */

	QString action_name;
	if (items.size() > 1) {
		action_name = QTStr("Undo.Sources.Multi").arg(QString::number(items.size()));
	} else {
		QString str = QTStr("Undo.Delete");
		action_name = str.arg(obs_source_get_name(obs_sceneitem_get_source(items[0])));
	}

	CreateSceneUndoRedoAction(action_name, undo_data, redo_data);
}


void OBSBasic::updateSystem() {
	CheckForUpdates(true);
}

void OBSBasic::activeClose()
{
	//FIXME 不建议直接调用 closeEvent，而是调用 close() 方法，因为它会自动触发 closeEvent。但为什么没有触发呢？
	QCloseEvent event;  // 构造一个关闭事件
	closeEvent(&event); // 手动调用 closeEvent 方法
}


void OBSBasic::addGuarderCtrlScene()
{
	std::string name = "场控实时对讲";
	OBSSceneAutoRelease scene = obs_scene_create(name.c_str());	
	obs_source_t *scene_source = obs_scene_get_source(scene);
	SetCurrentScene(scene_source);
	addMicrophoneSource();
	//addSpeakerSource();
}

void OBSBasic::removeGuarderCtrlScene()
{
	//RemoveSelectedScene(true);
	cleanGuarderCtrlScene();
}

void OBSBasic::cleanGuarderCtrlScene()
{
	auto enumScenes = [](void *param, obs_source_t *source) {
		if (source != nullptr) {
			OBSBasic *basic = (OBSBasic *)param;
			const char *name = obs_source_get_name(source);
			OBSScene scene = obs_scene_from_source(source);
			if ((name != nullptr) && (scene != nullptr)) {
				qDebug() << "name " << name;
				std::string sceneName = name;
				std::string dstName = "场控实时对讲";

				if (sceneName.compare(0, dstName.size(), dstName.c_str()) == 0) {
					basic->SetCurrentScene(scene);
					basic->RemoveSelectedScene(true);
					return false;
				}
			}
		}

		return true;
	};
	obs_enum_scenes(enumScenes, this);

}

OBSScene OBSBasic::querySceneBySceneName(std::string sceneName) {
	struct EnumData{
		std::string sceneName;
		OBSScene scene;
	}enumData;

	enumData.sceneName = sceneName;
	enumData.scene = nullptr;
	auto enumScenes = [](void *param, obs_source_t *source) {
		if (source != nullptr) {
			struct EnumData *data = (struct EnumData *)(param);
			std::string sceneName = data->sceneName;
			OBSBasic *basic = (OBSBasic *)param;
			const char *name = obs_source_get_name(source);
			OBSScene scene = obs_scene_from_source(source);
			
			if ((name != nullptr) && (scene != nullptr)) {
				qDebug() << "name " << name;
				std::string dstName = name;

				if (sceneName.compare(0, dstName.size(), dstName.c_str()) == 0) {
					/*basic->SetCurrentScene(scene);
					basic->RemoveSelectedScene(true);*/
					data->scene = scene;
					return false;
				}
			}
		}

		return true;
	};
	obs_enum_scenes(enumScenes, (void *)(&enumData));
	return enumData.scene;
}



void OBSBasic::activeIntercom(bool val)
{
	QMetaObject::invokeMethod(this, [&val, this]() {
		OBSSceneItem sceneitem = GetCurrentSceneItem();
		obs_scene_t *scene = obs_sceneitem_get_scene(sceneitem);
		obs_source_t *scenesource = obs_scene_get_source(scene);
		int64_t id = obs_sceneitem_get_id(sceneitem);
		const char *name = obs_source_get_name(scenesource);
		const char *uuid = obs_source_get_uuid(scenesource);
		obs_source_t *source = obs_sceneitem_get_source(sceneitem);
		#if 0
		auto undo_redo = [](const std::string &uuid, int64_t id, bool val) {
			OBSSourceAutoRelease s = obs_get_source_by_uuid(uuid.c_str());
			obs_scene_t *sc = obs_group_or_scene_from_source(s);
			obs_sceneitem_t *si = obs_scene_find_sceneitem_by_id(sc, id);
			if (si)
				obs_sceneitem_set_visible(si, val);
		};

		QString str = QTStr(val ? "Undo.ShowSceneItem" : "Undo.HideSceneItem");

		OBSBasic *main = OBSBasic::Get();
		main->undo_s.add_action(str.arg(obs_source_get_name(source), name),
					std::bind(undo_redo, std::placeholders::_1, id, !val),
					std::bind(undo_redo, std::placeholders::_1, id, val), uuid, uuid);
		#endif

		QSignalBlocker sourcesSignalBlocker(this);
		obs_sceneitem_set_visible(sceneitem, val);
	});
}


OBSSource OBSBasic::addSlideShowSource(QStringList files)
{
	size_t idx = 0;
	const char *unversioned_type;
	const char *type;
	OBSSource newSource;
	while (obs_enum_input_types2(idx++, &type, &unversioned_type)) {
		const char *name = obs_source_get_display_name(type);
		uint32_t caps = obs_get_source_output_flags(type);

		if (strcmp(type, "slideshow") == 0) {
			//AddSource(unversioned_type);
			//AddNew
			OBSSourceAutoRelease source = obs_get_source_by_name("幻灯片去重");
			if (source) {
				
			} else {

				OBSSceneItem newSceneItem;
				const char *v_id = obs_get_latest_input_type_id("slideshow");
				source = obs_source_create(v_id, "幻灯片去重", NULL, nullptr);
				//OBSScene scene = GetCurrentScene();
				OBSScene scene = querySceneBySceneName("场景");
				if (source && scene) {
					AddSourceData2 data;
					data.source = source;
					data.visible = true;

					obs_enter_graphics();
					obs_scene_atomic_update(scene, AddSource2, &data);
					obs_leave_graphics();

					newSource = source;
					newSceneItem = data.scene_item;

					/* set monitoring if source monitors by default */
					uint32_t flags = obs_source_get_output_flags(source);
					if ((flags & OBS_SOURCE_MONITOR_BY_DEFAULT) != 0) {
						obs_source_set_monitoring_type(source,
									       OBS_MONITORING_TYPE_MONITOR_ONLY);
					}

					bool closed = true;
					if (properties)
						closed = properties->close();
					obs_data_t *settings = obs_source_get_settings(source);
					OBSDataArrayAutoRelease array = obs_data_array_create();

	
					for (int i = 0; i < files.count(); i++) {
						OBSDataAutoRelease arrayItem = obs_data_create();
						obs_data_set_string(arrayItem, "value", files[i].toUtf8().constData());						
						obs_data_array_push_back(array, arrayItem);	
					}
					obs_data_set_array(settings, "files", array);
					obs_source_update(source, settings);

					if (!closed) {
						properties = new OBSBasicProperties(this, newSource);
						properties->Init();
						properties->setAttribute(Qt::WA_DeleteOnClose, true);
					}
				}
			}
		}
	}
	return newSource;
}


void OBSBasic::removeSlideShowSource() {
	size_t idx = 0;
	const char *unversioned_type;
	const char *type;
	OBSSource newSource;
	while (obs_enum_input_types2(idx++, &type, &unversioned_type)) {
		const char *name = obs_source_get_display_name(type);
		uint32_t caps = obs_get_source_output_flags(type);

		if (strcmp(type, "slideshow") == 0) {
			//AddSource(unversioned_type);
			//AddNew
			OBSSourceAutoRelease source = obs_get_source_by_name("幻灯片去重");
			if (source) {
				OBSSceneItem newSceneItem;
				const char *v_id = obs_get_latest_input_type_id("slideshow");
				OBSScene scene = querySceneBySceneName("场景");
				if (source && scene) {
					obs_source_remove(source);
					obs_source_release(source);
				}
			}
		}
	}
}

OBSSource OBSBasic::addTimeClockSource(QString file) {
	size_t idx = 0;
	const char *unversioned_type;
	const char *type;
	OBSSource newSource;
	while (obs_enum_input_types2(idx++, &type, &unversioned_type)) {
		const char *name = obs_source_get_display_name(type);
		uint32_t caps = obs_get_source_output_flags(type);

		if (strcmp(type, "browser_source") == 0) {
			//AddSource(unversioned_type);
			//AddNew
			OBSSourceAutoRelease source = obs_get_source_by_name("时钟去重");
			if (source) {

			} else {

				OBSSceneItem newSceneItem;
				const char *v_id = obs_get_latest_input_type_id("browser_source");
				source = obs_source_create(v_id, "时钟去重", NULL, nullptr);
				//OBSScene scene = GetCurrentScene();
				OBSScene scene = querySceneBySceneName("场景");
				if (source && scene) {
					AddSourceData2 data;
					data.source = source;
					data.visible = true;

					obs_enter_graphics();
					obs_scene_atomic_update(scene, AddSource2, &data);
					obs_leave_graphics();

					newSource = source;
					newSceneItem = data.scene_item;

					/* set monitoring if source monitors by default */
					uint32_t flags = obs_source_get_output_flags(source);
					if ((flags & OBS_SOURCE_MONITOR_BY_DEFAULT) != 0) {
						obs_source_set_monitoring_type(source,
									       OBS_MONITORING_TYPE_MONITOR_ONLY);
					}

					bool closed = true;
					if (properties)
						closed = properties->close();
					obs_data_t *settings = obs_source_get_settings(source);
					obs_data_set_bool(settings, "is_local_file",true);
					obs_data_set_string(settings, "local_file", file.toUtf8().constData());
		

					obs_source_update(source, settings);

					if (!closed) {
						properties = new OBSBasicProperties(this, newSource);
						properties->Init();
						properties->setAttribute(Qt::WA_DeleteOnClose, true);
					}
				}
			}
		}
	}
	return newSource;
}
void OBSBasic::removeTimeClockSource() {
	size_t idx = 0;
	const char *unversioned_type;
	const char *type;
	OBSSource newSource;
	while (obs_enum_input_types2(idx++, &type, &unversioned_type)) {
		const char *name = obs_source_get_display_name(type);
		uint32_t caps = obs_get_source_output_flags(type);

		if (strcmp(type, "browser_source") == 0) {
			//AddSource(unversioned_type);
			//AddNew
			OBSSourceAutoRelease source = obs_get_source_by_name("时钟去重");
			if (source) {
				OBSSceneItem newSceneItem;
				const char *v_id = obs_get_latest_input_type_id("browser_source");
				OBSScene scene = querySceneBySceneName("场景");
				if (source && scene) {
					obs_source_remove(source);
					obs_source_release(source);
				}
			}
		}
	}
}


void OBSBasic::changeTransform(int factor) {
	float fact = 1 - factor / 1000.0;
	float shiftPos = factor / 10;
	const auto item = GetCurrentSceneItem();
	if (!item)
		return;
	obs_source_t *source = obs_sceneitem_get_source(item);
	uint32_t source_cx = obs_source_get_width(source);
	uint32_t source_cy = obs_source_get_height(source);
	double width = double(source_cx);
	double height = double(source_cy);

	obs_transform_info oti;
	obs_sceneitem_get_info2(item, &oti);

	/* do not scale a source if it has 0 width/height */
	if (source_cx != 0 && source_cy != 0) {
		oti.scale.x = fact;
		oti.scale.y = fact;
	}

	oti.pos.x = float(oti.pos.x + shiftPos);
	oti.pos.y = float(oti.pos.y + shiftPos);
	oti.rot = float(shiftPos);

	
	oti.bounds.x = float(oti.bounds.x*fact);
	oti.bounds.y = float(oti.bounds.y * fact);
	
	obs_sceneitem_set_info2(item, &oti);
	
}


void OBSBasic::changeOpacity(int opacity) {
	const char *type_str;
	size_t idx = 0;
	double opa = opacity / 100.0;
	while (obs_enum_filter_types(idx++, &type_str)) {
		const char *name = obs_source_get_display_name(type_str);
		uint32_t caps = obs_get_source_output_flags(type_str);
		

		if ((caps & OBS_SOURCE_DEPRECATED) != 0)
			continue;
		if ((caps & OBS_SOURCE_CAP_DISABLED) != 0)
			continue;
		if ((caps & OBS_SOURCE_CAP_OBSOLETE) != 0)
			continue;

	}
	OBSScene scene = GetCurrentScene();
	OBSSource source = obs_scene_get_source(scene);
	OBSDataAutoRelease settings = obs_source_get_settings(source);
	obs_data_set_double(settings, "opacity", opa);


}



void OBSBasic::StartGBSStreaming(std::string server, std::string key)
{
	service = obs_service_create("rtmp_custom", "default_service", nullptr, nullptr);
	if (!service)
		return;
	obs_service_release(service);

	obs_data_t *settings1 = obs_service_get_settings(service);
	obs_data_set_string(settings1, "server",
			    server.c_str()); // 设置 RTMP 服务器地址
	obs_data_set_string(settings1, "key",
			    key.c_str()); // 设置推流码 (Stream Key)

	obs_service_update(service, settings1);
	obs_data_release(settings1);

	if (outputHandler->StreamingActive())
		return;

	auto finish_stream_setup = [&](bool setupStreamingResult) {
		if (!setupStreamingResult) {
			return;
		}
		//OnEvent(OBS_FRONTEND_EVENT_STREAMING_STARTING);

		if (!outputHandler->StartStreaming(service)) {
			return;
		}

		bool recordWhenStreaming =
			config_get_bool(App()->GetUserConfig(), "BasicWindow", "RecordWhenStreaming");

		bool replayBufferWhileStreaming =
			config_get_bool(App()->GetUserConfig(), "BasicWindow", "ReplayBufferWhileStreaming");
		if (replayBufferWhileStreaming)
			StartReplayBuffer();

#ifdef YOUTUBE_ENABLED
		if (!autoStartBroadcast)
			OBSBasic::ShowYouTubeAutoStartWarning();
#endif
	};

	setupStreamingGuard = outputHandler->SetupStreaming(service, finish_stream_setup);
}

void OBSBasic::StopGBSStreaming()
{
	OBSBasic *main = OBSBasic::Get();
	main->SaveProject();

	if (outputHandler->StreamingActive())
		outputHandler->StopStreaming(true);

	bool recordWhenStreaming = config_get_bool(App()->GetUserConfig(), "BasicWindow", "RecordWhenStreaming");
	bool keepRecordingWhenStreamStops =
		config_get_bool(App()->GetUserConfig(), "BasicWindow", "KeepRecordingWhenStreamStops");
	if (recordWhenStreaming && !keepRecordingWhenStreamStops)
		main->StopRecording();

	bool replayBufferWhileStreaming =
		config_get_bool(App()->GetUserConfig(), "BasicWindow", "ReplayBufferWhileStreaming");
	bool keepReplayBufferStreamStops =
		config_get_bool(App()->GetUserConfig(), "BasicWindow", "KeepReplayBufferStreamStops");
	if (replayBufferWhileStreaming && !keepReplayBufferStreamStops)
		main->StopReplayBuffer();
}


void OBSBasic::checkGBSForUpdate(bool manualUpdate)
{
	if (updateGBSCheckThread && updateGBSCheckThread->isRunning())
		return;
	updateGBSCheckThread.reset(new GBSAutoUpdateThread(manualUpdate));
	updateGBSCheckThread->start();

	//升级分两种情况，
	//1. 程序启动完成之后，自动开始升级线程，这里会判断是否有文件可以更新，如果有的话自动下载，下载完成后弹框提示[提示升级内容]，
	// 有新的升级包需要安装，有现在和稍后两个选项，如果用户点击现在退出程序，马上升级。否则下次启动执行进行升级。
	//2. 程序启动后，已经登录成功，用户在Profile里面点击软件升级，属于手动升级，这个时候弹框提示[提示升级内容] 有确定和取消两个按钮选择。
	// 如果用户点击了确认，程序马上退出，并进行升级，否则，等待下次程序启动升级。



	connect((GBSAutoUpdateThread *)(updateGBSCheckThread.get()), &GBSAutoUpdateThread::sigHasNewerVersion,
		[this](QList<QString> features) {
			QMetaObject::invokeMethod(this, "showNewFeaturesDialog", Qt::QueuedConnection,
						  Q_ARG(QList<QString>, features));

		});

}

void OBSBasic::showNewFeaturesDialog(QList<QString> features) {
	QString textEditStyle = R"(
	    QTextEdit {
		background-color: #f9f9f9;            /* 背景色 */
		border: 1px solid #cccccc;            /* 边框 */
		border-radius: 4px;                  /* 圆角 */
		padding: 8px;                         /* 内边距 */
		font-size: 14px;                      /* 字体大小 */
		font-family: "Segoe UI", sans-serif;   /* 字体 */
		color: #333333;                       /* 文本颜色 */
		selection-background-color: #6ec1e4;  /* 选中文本背景 */
		selection-color: white;               /* 选中文本颜色 */
	    }
    
	    QTextEdit:focus {
		border-color: #3d9b7f;                /* 聚焦时的边框颜色 */
		box-shadow: 0 0 10px rgba(61, 155, 127, 0.6); /* 聚焦时的阴影效果 */
	    }
    
	    QTextEdit::cursor {
		width: 2px;
		background-color: #6ec1e4;            /* 光标颜色 */
	    }
	)";

	QWidget *widget = new QWidget();
	QVBoxLayout *layout = new QVBoxLayout(widget);
	layout->setAlignment(Qt::AlignHCenter); // 整体内容居中

	// Spacer
	QSpacerItem *spacer0 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Minimum);

	// 创建 QTextEdit 组件
	QTextEdit *featureTexts = new QTextEdit();
	featureTexts->setFixedWidth(480);
	featureTexts->setFixedHeight(320);

	featureTexts->setReadOnly(true);

	// 保证 QTextEdit 组件本身居中
	featureTexts->setAlignment(Qt::AlignLeft); // 保证文本是左对齐
	featureTexts->setStyleSheet(textEditStyle);

	// 构造 featureAttrs 文本内容
	QString featureAttrs;
	for (auto feature : features) {
		featureAttrs += feature;
		featureAttrs += "\n";
	}
	featureTexts->setPlainText(featureAttrs);

	// 设置滚动条
	QScrollBar *scrollBar = featureTexts->verticalScrollBar();
	scrollBar->setSingleStep(1);
	scrollBar->setPageStep(3);

	// 将 QTextEdit 添加到布局中，确保它在布局中居中
	layout->addWidget(featureTexts, Qt::AlignVCenter);

	QString btnStyle = R"(
		QPushButton {
			background-color:#00C566;
			border: 1px solid white;
			border-radius:10px;
			font-size:20px;
		}

		QPushButton:pressed {
			background-color: #D1D8DD;
			padding-left: 3px;"       
			padding-top: 3px;"        
			background-repeat: no-repeat;
			background-position: center;
		}
	)";
	    


	QPushButton *cancel = new QPushButton("取消");
	QPushButton *ok = new QPushButton("确认");
	ok->setFixedSize(QSize{90, 48});
	cancel->setFixedSize(QSize{90, 48});
	ok->setStyleSheet(btnStyle);
	cancel->setStyleSheet(btnStyle);

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(cancel);
	buttonLayout->addWidget(ok);
	layout->addLayout(buttonLayout);

	// Spacer
	QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	layout->addSpacerItem(spacer);



	// 创建对话框
	GBSMsgDialog *dialog = new GBSMsgDialog("有新的版本可以更新", layout);
	connect(cancel, &QPushButton::clicked, dialog, &QDialog::reject);
	connect(ok, &QPushButton::clicked, this, [dialog, this]() {
		if (updateGBSCheckThread && updateGBSCheckThread->isRunning())
			return;
		updateGBSCheckThread.reset(new GBSAutoUpdateThread(false));
		updateGBSCheckThread->start();
		dialog->accept();
		connect((GBSAutoUpdateThread *)(updateGBSCheckThread.get()), &GBSAutoUpdateThread::sigDownloadFinished,
			[this]() { QMetaObject::invokeMethod(this, "showNewReleaseDialog", Qt::QueuedConnection); });

	});
	dialog->exec();
}

void OBSBasic::showNewReleaseDialog() {
	QWidget *widget = new QWidget();
	QVBoxLayout *layout = new QVBoxLayout(widget);
	layout->setAlignment(Qt::AlignHCenter); // 整体内容居中
	QSpacerItem *spacer0 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Minimum);
	QTextEdit *featureTexts = new QTextEdit();
	featureTexts->setReadOnly(true);

	QScrollBar *scrollBar = featureTexts->verticalScrollBar();
	scrollBar->setSingleStep(1);
	scrollBar->setPageStep(3);

	QPushButton *cancel = new QPushButton("取消");
	QPushButton *ok = new QPushButton("确认");

	ok->setFixedSize(QSize{90, 48});
	cancel->setFixedSize(QSize{90, 48});
	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(cancel);
	buttonLayout->addWidget(ok);
	layout->addLayout(buttonLayout);

	QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	layout->addSpacerItem(spacer);
	GBSMsgDialog *dialog = new GBSMsgDialog("立刻马上重启应用，完成升级", layout);
	connect(cancel, &QPushButton::clicked, dialog, &QDialog::reject);
	connect(ok, &QPushButton::clicked, this, [dialog, this]() {
		std::filesystem::path exePath = std::filesystem::absolute(std::filesystem::current_path());
		std::string exePathStr = exePath.string();
		QString qExePath = QString::fromLocal8Bit(exePathStr);
		QString qExePathFile = qExePath  + "\\update.lock";
		QFile file(qExePathFile);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			file.close();
		}

		QMetaObject::invokeMethod(App()->GetMainWindow(), "close");
		dialog->accept();
	});
	dialog->exec();
}



