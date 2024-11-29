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

#include <qt-wrappers.hpp>
#include "gbs/common/FatButton.h"
#include "gbs/common/QIniFile.h"
#include "gbs/common/GBSHttpClient.h"
#include "gbs/common/QBizLogger.h"
#include "gbs/naviWidgets/GBSNaviData.h"



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
static QPixmap getRoundedPixmap(const QPixmap &src, int diameter) {
    // 创建一个目标 QPixmap，设置为透明背景
    QPixmap dst(diameter, diameter);
    dst.fill(Qt::transparent);

    // 缩放源图像到适合的大小，保持抗锯齿和平滑转换
    QPixmap scaledSrc = src.scaled(diameter, diameter, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    // 使用 QPainter 绘制裁剪后的圆形图片
    QPainter painter(&dst);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // 创建圆形裁剪路径并设置裁剪区域
    QPainterPath path;
    path.addEllipse(0, 0, diameter, diameter);
    painter.setClipPath(path);

    // 将缩放后的图片绘制到目标图像上
    QRect targetRect(0, 0, diameter, diameter);
    QRect sourceRect((scaledSrc.width() - diameter) / 2, 
                     (scaledSrc.height() - diameter) / 2, 
                     diameter, diameter);
    painter.drawPixmap(targetRect, scaledSrc, sourceRect);

    return dst;
}


void OBSBasic::onUserInfo(const GBSUserInfo *info) {
	if (!info->GetHead().empty()) {
		GBSHttpClient::getInstance()->downFile(info->GetHead(), "avator.png", 0);
	}
	
}

void OBSBasic::onUserFileDownLoad(const std::string &path, int type) {
	QMetaObject::invokeMethod(this, [path,type, this]() {
		if (type == 0 && !path.empty()) {
			QString qPath = QString::fromStdString(path);
			QPixmap rounded = getRoundedPixmap(qPath, 64);

			rounded.save("round-avator.png");
			QString appDirPath = QCoreApplication::applicationDirPath();
			QLogD("onUserFileDownLoad, %s.", appDirPath.toStdString().c_str());

			appDirPath += "/round-avator.png";
			emit onUseIconUpdate(qPath);
		}
		});


}

QString OBSBasic::getRoundedAvator() {
	QString file = QCoreApplication::applicationDirPath() + "/avator.png";
	QFile qFile(file);
	if (qFile.exists()) {
		return file;
	} else {
		return QCoreApplication::applicationDirPath() + "/avator.png";
	}
}

void OBSBasic::onRoomInfos(std::list<GBSRoomInfo> &info) {}

void OBSBasic::onRoomInfo(GBSRoomInfo *info) {}
void OBSBasic::onQRcodeInfo(std::string no, std::string url, int status){}

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
	std::string name = "场控对讲";
	OBSSceneAutoRelease scene = obs_scene_create(name.c_str());	
	obs_source_t *scene_source = obs_scene_get_source(scene);
	SetCurrentScene(scene_source);
	addMicrophoneSource();
	addSpeakerSource();
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
				std::string dstName = "场控对讲";

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
