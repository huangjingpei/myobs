#include <string>
#include <algorithm>
#include <cinttypes>
#include "window-basic-main.hpp"

#include <qt-wrappers.hpp>
#include "gbs/common/FatButton.h"
#include "gbs/common/QIniFile.h"
#include "gbs/common/GBSHttpClient.h"
#include "gbs/common/QBizLogger.h"


// 本窗体是为了初始化自己窗口的函数，区别于OBSInit的是: 后者是初始化OBS系统，前者只是简单的初始化一个窗口
// 这里你也可以理解为OBSBasic 及负责了整个除登录页面生命周期意外的所有软件的生命周期

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
}

void OBSBasic::OBSDeinit2() {
	QLogD("OBSDeinit2, End pull task.");
	GBSHttpClient::getInstance()->unRegisterHandler(this);
	endPullTask();
	
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
	pullStreamTimer = new QTimer(this);
	connect(pullStreamTimer.data(), &QTimer::timeout, this,
		[this]() {
			GBSHttpClient::getInstance()->getPullStream();
			QPoint center = this->rect().center(); // 获取窗口的矩形区域的中心点

			//QScopedPointer<QMenu> popup(CreateAddSourcePopupMenu());
			//if (popup) {
			//	popup->exec(center);
			//}

			
		});
	pullStreamTimer->start(5000);
}
void OBSBasic::endPullTask() {
	QLogD("End pull task.");
	if (pullStreamTimer != nullptr) {
		pullStreamTimer->stop();
		delete pullStreamTimer;
		pullStreamTimer = nullptr;
	}
}
void OBSBasic::onLoginResult(const int result) {}

void OBSBasic::onRtmpPushUrl(const std::string url) {}

void OBSBasic::onPullRtmpUrl(const std::string url) {
	qDebug() << "onPullRtmpUrl " << url;
	if (!url.empty()) {
		QString newUrl = QString::fromStdString(url);
		if (pullRtmpUrl != newUrl) {
			pullRtmpUrl = newUrl;
			QMetaObject::invokeMethod(this, "startPullStream", Q_ARG(QString, pullRtmpUrl));

		}
	}
}
void OBSBasic::startPullStream(QString rtmp) {
	size_t idx = 0;
	const char *unversioned_type;
	const char *type;
	if (rtmp.startsWith("rtmp://",Qt::CaseInsensitive) == 0) {
		return;
	}
	while (obs_enum_input_types2(idx++, &type, &unversioned_type)) {
		const char *name = obs_source_get_display_name(type);
		uint32_t caps = obs_get_source_output_flags(type);

		if (strcmp(type, "ffmpeg_source") == 0) {
			//AddSource(unversioned_type);
			//AddNew
			OBSSourceAutoRelease source = obs_get_source_by_name("RTMP 矩阵地址");
			if (source) {

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





void OBSBasic::onUserInfo(const GBSUserInfo *info) {}

void OBSBasic::onUserIconPath(const std::string &path) {}

void OBSBasic::onRoomInfos(std::list<GBSRoomInfo> &info) {}

void OBSBasic::onRoomInfo(GBSRoomInfo *info) {}


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
