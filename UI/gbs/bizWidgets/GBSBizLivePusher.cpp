#include "GBSBizLivePusher.h"
#include "ui_GBSBizLivePusher.h"
#include "qscrollbar.h"
#include <QTimer>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include "qt-wrappers.hpp"

#include "../common/DanmakuWidget.h"

#include "window-basic-main.hpp"
#include "display-helpers.hpp"
#include "gbs/common/QBizLogger.h"

/**
QFrame边框的样式
background-color:#F9F9F9;
border: 1px solid  #FFFFFF;
border-radius: 16px;



 */
#include "gbs/common/GBSHttpClient.h"
#include "qt-display.hpp"

#include "../GBSToolKits.h"





GBSBizLivePusher::GBSBizLivePusher(QWidget *parent) : QWidget(parent), ui(new Ui::GBSBizLivePusher)
{
	ui->setupUi(this);
	danmaindex = 0;
	//ui->dockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	//ui->dockWidget->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
	//ui->dockWidget->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);

	//ui->dockWidget->setStyleSheet("QDockWidget::title { background: transparent; }");

	// 创建阴影效果
	//QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect();
	//shadowEffect->setBlurRadius(4);          // 模糊半径
	//shadowEffect->setOffset(0, 10);          // 偏移 (水平 0px, 垂直 10px)
	//shadowEffect->setColor(QColor(12, 12, 12));  // 阴影颜色 #0C0C0C
	//ui->wgtPreview->setGraphicsEffect(shadowEffect);

	// 初始化布局
	danmaKuAreaLayout = new QVBoxLayout();
	danmaKuAreaLayout->setSpacing(6);                 // 设置widget之间的固定间距为6像素
	danmaKuAreaLayout->setAlignment(Qt::AlignBottom); // 保证最新widget在最底部

	QWidget *containerWidget = new QWidget(); // 用于容纳所有widgets
	// 为containerWidget设置一个类名
	containerWidget->setObjectName("ContainerWidget");
	containerWidget->setStyleSheet("#ContainerWidget {"
				       "   border: 2px solid #F9F9F9;"
				       "   border-radius: 5px;"
				       "}");

	containerWidget->setLayout(danmaKuAreaLayout);
	ui->tabWidget->setStyleSheet("QTabWidget::pane {"
				     "    border: none;" // 移除tab pane的边框
				     "}"
				     "QTabBar::tab {"
				     "    background: none;"   // tab的背景颜色
				     "    padding: 10px;"      // tab内容的填充
				     "    margin-right: 10px;" // 调整tab之间的水平间距
				     "    border: none;"       // 移除tab的边框
				     "    color: #01C667;"
				     "    font-size:16px;"
				     "}"
				     "QTabBar::tab:first {"
				     "    margin-left: 60px;" // 调整第一个tab项的左外边距
				     "}"
				     "QTabBar::tab:selected {"
				     "    background: #C0C0C0;" // 选中tab的背景颜色
				     "    border: none;"        // 选中时也不显示边框
				     "    border-radius: 16px"
				     "}"
				     "QTabBar {"
				     "    qproperty-alignment: AlignCenter;" // 设置tab项居中对齐
				     "}");

	// ScrollArea 包裹 widgets 的容器
	danmakuscrollArea = new QScrollArea(this);
	danmakuscrollArea->setWidget(containerWidget);
	danmakuscrollArea->setWidgetResizable(true);
	danmakuscrollArea->setStyleSheet("QScrollArea { border: none; }"
					 "QScrollBar:vertical { width: 10px; }" // 调整垂直滚动条的宽度
					 "QScrollBar::handle:vertical { background: #CCCCCC; }" // 滚动条手柄颜色
	);

	ui->verticalLayout->addWidget(danmakuscrollArea);

	ui->btnAgainstRule->setStyleSheet("border-image:url(:gbs/images/gbs/biz/gbs-widget-docking.png)");

	ui->btnStartLive->setStyleSheet("border-image:url(:gbs/images/gbs/biz/gbs-start-living.png)");

	ui->wgtPreview->setStyleSheet("QWidget {"
				      "   background: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, "
				      "       stop:0 #000000, stop:0.525 #434343, stop:1 #000000);" // 渐变背景
				      "   border-radius: 20px;"                                     // 圆角20px
				      "   color: white;"                                            // 文本颜色
				      "   font-size: 16px;"                                         // 字体大小
				      "   padding: 10px;"                                           // 内边距
				      "}");

	auto displayResize = [this]() {
		struct obs_video_info ovi;

		if (obs_get_video_info(&ovi))
			ResizePreview(ovi.base_width, ovi.base_height);
	};
	connect(ui->wgtPreview, &OBSQTDisplay::DisplayResized, displayResize);

	size_t idx = 0;
	const char *unversioned_type;
	const char *type;
	while (obs_enum_input_types2(idx++, &type, &unversioned_type)) {
		const char *name = obs_source_get_display_name(type);
		uint32_t caps = obs_get_source_output_flags(type);

		if ((caps & OBS_SOURCE_CAP_DISABLED) != 0)
			continue;

		if ((caps & OBS_SOURCE_DEPRECATED) == 0) {
			//addsource(popup, unversioned_type, name);
			qDebug() << "idx+++0 " << idx << " type " << type << " name " << name << " unversioned_type "
				 << unversioned_type;

		} else {
			qDebug() << "idx+++1 " << idx << " type " << type << " name " << name << " unversioned_type "
				 << unversioned_type;
		}
	}

	obs_enum_sources(FindCameraSource, this);
	if (cameraSource == nullptr) {
		//QMessageBox::StandardButton button = OBSMessageBox::question(this, "你还没有添加视频源", "请到代播设置页面添加");

		//if (button == QMessageBox::Yes) {
		//
		//}
	}

	currentCameraIndex = -1;
	std::list<QString> devices = GBSToolKits::getInstance()->EnumDevice(cameraSource);
	for (auto it = devices.begin(); it != devices.end(); it++) {
		QString name = (*it);
		int index = name.indexOf(":");
		QString leftName = name.left(index);
		ui->cbxCamera->addItem(leftName);
		currentCameraIndex = 0;
	}
	std::list<QString> audioDevices = GBSToolKits::getInstance()->EnumAudioDevice(cameraSource);
	for (auto it = audioDevices.begin(); it != audioDevices.end(); it++) {
		QString name = (*it);
		int index = name.indexOf(":");
		QString leftName = name.left(index);
		ui->cbxMicrophone->addItem(leftName);
	}
	//if (currentCameraIndex >= 0) {
	//	GBSToolKits::getInstance()->StartPreview(cameraSource, currentCameraIndex);
	//}

	////cameraSource = GBSToolKits::getInstance()->CreateSource(
	////	"dshow_input", "DshowWindowsCapture");
	////currentCameraIndex = -1;
	////std::list<QString> devices =
	////	GBSToolKits::getInstance()->EnumDevice(cameraSource);
	////for (auto it = devices.begin(); it != devices.end(); it++) {
	////	QString name = (*it);
	////	int index = name.indexOf(":");
	////	QString leftName = name.left(index);
	////	ui->cbxCamera->addItem(leftName);
	////	currentCameraIndex = 0;
	////}
	////std::list<QString> audioDevices =
	////	GBSToolKits::getInstance()->EnumAudioDevice(cameraSource);
	////for (auto it = audioDevices.begin(); it != audioDevices.end(); it++) {
	////	QString name = (*it);
	////	int index = name.indexOf(":");
	////	QString leftName = name.left(index);
	////	ui->cbxMicrophone->addItem(leftName);
	////}
	////if (currentCameraIndex >= 0) {
	////	GBSToolKits::getInstance()->StartPreview(cameraSource,
	////						 currentCameraIndex);
	////}
	////
	connect(ui->cbxCamera, &QComboBox::currentIndexChanged, this, &GBSBizLivePusher::onComboBoxIndexChanged);
	connect(ui->cbxCamera, &QComboBox::currentIndexChanged, this, &GBSBizLivePusher::onComboBoxIndexChanged);

	auto addDisplay = [this](OBSQTDisplay *window) {
		OBSBasic *main = reinterpret_cast<OBSBasic *>(App()->GetMainWindow());
		obs_display_add_draw_callback(window->GetDisplay(), GBSBizLivePusher::RenderMain, this);

		struct obs_video_info ovi;
		if (obs_get_video_info(&ovi))
			ResizePreview(ovi.base_width, ovi.base_height);
	};

	ui->wgtPreview->Init();
	connect(ui->wgtPreview, &OBSQTDisplay::DisplayCreated, addDisplay);
	connect(ui->btnStartLive, &QPushButton::clicked, this, &GBSBizLivePusher::onStartRtmpPush);

	OBSBasic *main = reinterpret_cast<OBSBasic *>(App()->GetMainWindow());
	outputHandler.reset();
	outputHandler.reset(CreateSimpleOutputHandler(main));

	GBSHttpClient::getInstance()->registerHandler(this);
	GBSHttpClient::getInstance()->pageQuery();

	ui->btnAll->setCheckable(true);  // 允许按钮被选中
	ui->btnFans->setCheckable(true); // 允许按钮被选中
	ui->btnTips->setCheckable(true); // 允许按钮被选中
	ui->btnF2F->setCheckable(true);  // 允许按钮被选中

	connect(ui->btnAll, &QPushButton::toggled, this, &GBSBizLivePusher::updateStyle);
	connect(ui->btnFans, &QPushButton::toggled, this, &GBSBizLivePusher::updateStyle);
	connect(ui->btnTips, &QPushButton::toggled, this, &GBSBizLivePusher::updateStyle);
	connect(ui->btnF2F, &QPushButton::toggled, this, &GBSBizLivePusher::updateStyle);
	btnDanmaLists.append(ui->btnAll);
	btnDanmaLists.append(ui->btnFans);
	btnDanmaLists.append(ui->btnTips);
	btnDanmaLists.append(ui->btnF2F);
	for (QPushButton *item : btnDanmaLists) {
		item->setStyleSheet("QPushButton {"
				    "   border-radius: 5px;" // 圆角
				      "   color: #78828A;"
				      "   border: black;" // 无边框
				      "}"

				      "QPushButton:pressed {"
				      "   background-color: #D1D8DD;" // 按下时背景颜色
				      "   padding-left: 3px;"         // 向左移动 3px
				      "   padding-top: 3px;"          // 向上移动 3px
				      "   background-repeat: no-repeat;"
				      "   background-position: center;"
				      "}");
	}
	ui->btnAll->setStyleSheet("QPushButton {"
				 "   border-radius: 5px;" // 圆角
			      "   color: #00c566;"
			      "   font-size: 10px;"
			      "}"

			      "QPushButton:pressed {"
			      "   background-color: #D1D8DD;" // 按下时背景颜色
			      "   padding-left: 3px;"         // 向左移动 3px
			      "   padding-top: 3px;"          // 向上移动 3px
			      "   background-repeat: no-repeat;"
			      "   background-position: center;"
			      "}");
}

void GBSBizLivePusher::updateStyle(bool checked) {
	QPushButton *button = qobject_cast<QPushButton *>(sender());
	int i = 0;
	for (QPushButton *item : btnDanmaLists) {
		++i;
		if ((button == item)) {
			qDebug() << "index " << i << "selected";
			item->setStyleSheet("QPushButton {"
					    "   border-radius: 5px;" // 圆角
					      "   color: #00c566;"
					      "   font-size: 10px;"
					      "}"

					      "QPushButton:pressed {"
					      "   background-color: #D1D8DD;" // 按下时背景颜色
					      "   padding-left: 3px;"         // 向左移动 3px
					      "   padding-top: 3px;"          // 向上移动 3px
					      "   background-repeat: no-repeat;"
					      "   background-position: center;"
					      "}");
		} else {
			qDebug() << "index " << i << "un-selected";
			item->setStyleSheet("QPushButton {"
					    "   border-radius: 5px;" // 圆角
					      "   color: #78828A;"
					    "   font-size: 10px;"
					      "}"

					      "QPushButton:pressed {"
					      "   background-color: #D1D8DD;" // 按下时背景颜色
					      "   padding-left: 3px;"         // 向左移动 3px
					      "   padding-top: 3px;"          // 向上移动 3px
					      "   background-repeat: no-repeat;"
					      "   background-position: center;"
					      "}");
		}
	}

}

bool GBSBizLivePusher::FindCameraSource(void *param, obs_source_t *source)
{
	GBSBizLivePusher *self = reinterpret_cast<GBSBizLivePusher *>(param);
	uint32_t flags = obs_source_get_output_flags(source);
	const char *name = obs_source_get_name(source);
	const char *id = obs_source_get_id(source);
	const char *uuid = obs_source_get_uuid(source);
	obs_data_t *settings = obs_source_get_settings(source);

	if ((id != nullptr)
		&& ((strcmp(id, "dshow_input") == 0))) {
		self->cameraSource = source;
		return false;
	}
	qDebug() << "flags " << flags << " name " << name << " id " << id
		 << " uuid " << uuid << " settings "
		 << obs_data_get_json(settings);

	return true;
}



void GBSBizLivePusher::onStartRtmpPush() {
	selectIDSDialog->show();
	std::string equipments = qEquipments.toStdString();
	GBSHttpClient::getInstance()->startLive(equipments);

	if (qPushUrl.isEmpty()) {
		return;
	}

	// 找到 "/live/" 的位置
	int liveIndex = qPushUrl.indexOf("/live/");

	// 如果找到了 "/live/"
	if (liveIndex != -1) {
		// 截取第一部分：从头到 "/live/" + 5的位置 ("/live/" 长度是5)
		QString serverUrl = qPushUrl.left(liveIndex + 5);

		// 截取第二部分：从 "/live/" 之后的部分
		QString key = qPushUrl.mid(liveIndex + 6);

		StartStreaming(serverUrl.toStdString(), key.toStdString());	
	}

	
}


void GBSBizLivePusher::ResizePreview(uint32_t cx, uint32_t cy)
{
	QSize targetSize;
	bool isFixedScaling;
	obs_video_info ovi;

	/* resize preview panel to fix to the top section of the window */
	targetSize = GetPixelSize(ui->wgtPreview);

	isFixedScaling = ui->wgtPreview->IsFixedScaling();
	obs_get_video_info(&ovi);

	if (isFixedScaling) {
		previewScale = ui->wgtPreview->GetScalingAmount();

		ui->wgtPreview->ClampScrollingOffsets();

		GetCenterPosFromFixedScale(
			int(cx), int(cy),
			targetSize.width() - PREVIEW_EDGE_SIZE * 2,
			targetSize.height() - PREVIEW_EDGE_SIZE * 2, previewX,
			previewY, previewScale);
		previewX += ui->wgtPreview->GetScrollX();
		previewY += ui->wgtPreview->GetScrollY();

	} else {
		GetScaleAndCenterPos(int(cx), int(cy),
				     targetSize.width() - PREVIEW_EDGE_SIZE * 2,
				     targetSize.height() -
					     PREVIEW_EDGE_SIZE * 2,
				     previewX, previewY, previewScale);
	}

	ui->wgtPreview->SetScalingAmount(previewScale);

	previewX += float(PREVIEW_EDGE_SIZE);
	previewY += float(PREVIEW_EDGE_SIZE);
}


void GBSBizLivePusher::RenderMain(void *data, uint32_t, uint32_t)
{
	GS_DEBUG_MARKER_BEGIN(GS_DEBUG_COLOR_DEFAULT, "RenderMain");

	GBSBizLivePusher *window = static_cast<GBSBizLivePusher *>(data);
	obs_video_info ovi;

	obs_get_video_info(&ovi);

	window->previewCX = int(window->previewScale * float(ovi.base_width));
	window->previewCY = int(window->previewScale * float(ovi.base_height));

	gs_viewport_push();
	gs_projection_push();

	obs_display_t *display = window->ui->wgtPreview->GetDisplay();
	uint32_t width, height;
	obs_display_size(display, &width, &height);
	float right = float(width) - window->previewX;
	float bottom = float(height) - window->previewY;

	gs_ortho(-window->previewX, right, -window->previewY, bottom, -100.0f,
		 100.0f);

	window->ui->wgtPreview->DrawOverflow();

	/* --------------------------------------- */

	gs_ortho(0.0f, float(ovi.base_width), 0.0f, float(ovi.base_height),
		 -100.0f, 100.0f);
	gs_set_viewport(window->previewX, window->previewY, window->previewCX,
			window->previewCY);

	{
		obs_render_main_texture_src_color_only();
	}
	gs_load_vertexbuffer(nullptr);

	/* --------------------------------------- */

	gs_ortho(-window->previewX, right, -window->previewY, bottom, -100.0f,
		 100.0f);
	gs_reset_viewport();

	uint32_t targetCX = window->previewCX;
	uint32_t targetCY = window->previewCY;



	window->ui->wgtPreview->DrawSceneEditing();


	/* --------------------------------------- */

	gs_projection_pop();
	gs_viewport_pop();

	GS_DEBUG_MARKER_END();
}




void GBSBizLivePusher::onComboBoxIndexChanged(int index) {
	if (currentCameraIndex != index) {
		if (cameraSource) {
			GBSToolKits::getInstance()->StopPreview(cameraSource);
			GBSToolKits::getInstance()->StartPreview(cameraSource,
								 index);
			currentCameraIndex = index;
		}
	}
}

//void GBSBizLivePusher::addNewWidget() {
//	++danmaindex;
//	QString text = "这是动态设置的第" + QString::number(danmaindex) + "行文本";
//	addNewWidget2(text, ":gbs/images/gbs/biz/gbs-logo-douyin.png", "文本会自动换行以适应窗口宽度。我这里就需要看到你换行,这是动态设置的第二行文本。文本会自动换行以适应窗口宽度。我这里就需要看到你换行");
//
//}

void GBSBizLivePusher::addNewWidget(const QString &text,
				    const QString &imagePath,
				    const QString &text2)
{
	// 创建新 widget
	DanmakuWidget *newWidget = new DanmakuWidget(this);
	newWidget->setFirstRowContent(text, imagePath);
	newWidget->setSecondRowContent(text2);

	// 添加到布局
	widgetList.append(newWidget);

	// 插入到布局的最底部
	danmaKuAreaLayout->insertWidget(danmaKuAreaLayout->count(), newWidget);  // 最新的 widget 添加到布局的最后
	qDebug() << "danmaKuAreaLayout count " << danmaKuAreaLayout->count();

	// 滚动到最底部显示最新添加的widget
	QScrollBar *vScrollBar = danmakuscrollArea->verticalScrollBar();
	vScrollBar->setValue(vScrollBar->maximum());

	// 检查是否超过 50 个 widget
	if (widgetList.size() > 30) {
		// 移除最早的 widget
		DanmakuWidget *oldestWidget = widgetList.takeFirst();
		danmaKuAreaLayout->removeWidget(oldestWidget);
		delete oldestWidget;
	}
}

void GBSBizLivePusher::onLoginResult(int result) {

}

void GBSBizLivePusher::onRtmpPushUrl(std::string url) {
	
	qDebug() << "GBSBizLivePusher URL: " << url;
	QLogD("Rtmp url ", url.c_str());


}

void GBSBizLivePusher::onPullRtmpUrl(std::string url) {
}

void GBSBizLivePusher::onUserInfo(const GBSUserInfo *info) {}

void GBSBizLivePusher::onUserIconPath(const std::string &path) {
	qDebug() << "GBSBizLivePusher onUserIconPath: " << path;
}

void GBSBizLivePusher::onRoomInfos(std::list<GBSRoomInfo>& infos) {
	QMetaObject::invokeMethod(this, [&infos, this]() {

		std::list<GBSRoomInfo> infosBak = infos;
		selectIDSDialog = new SelectedIDSDialog();
			qEquipments = "";
		for (auto info = infosBak.begin(); info != infosBak.end(); info++) {

			std::string s1 = (*info).getRoomId();
			std::string s2 = (*info).getSortCode();
			std::string s3 = std::to_string((*info).getId());
			QString qs1 = QString::fromStdString(s1);
			QString qs2 = QString::fromStdString(s2);
			QString qs3 = QString::fromStdString(s3);

			selectIDSDialog->addListItem(qs1 + " " + "(" + qs2 + ")", qs3);
			qEquipments += (*info).getEquipmentNo();
			qEquipments += ",";
		}

		if (!qEquipments.isEmpty()) {
			qEquipments.remove('{').remove('}');
			qEquipments = qEquipments.left(qEquipments.length() - 1);

		}
		connect(selectIDSDialog, &SelectedIDSDialog::dataSubmitted, this,
				&GBSBizLivePusher::dialogSubmitted);	
	}, Qt::QueuedConnection);

}

void GBSBizLivePusher::dialogSubmitted(const QString &data) {
	std::string ids = data.toStdString();
	GBSHttpClient::getInstance()->upRemoteLiveRoomStateTask(ids);
}
void GBSBizLivePusher::onRoomInfo(GBSRoomInfo *info)
{
	
	int32_t userId = info->getUserId();
	qUserId = QString("%1").arg(userId);
	qDebug() << "GBSBizLivePusher onRoomInfo: " << qUserId;
}

GBSBizLivePusher::~GBSBizLivePusher()
{
	service = nullptr;
	outputHandler.reset();
	obs_display_remove_draw_callback(ui->wgtPreview->GetDisplay(),
					 GBSBizLivePusher::RenderMain,
					 this);


	if (cameraSource) {
		//GBSToolKits::getInstance()->StopPreview(cameraSource);
	}
	
	OBSBasic *main = reinterpret_cast<OBSBasic *>(App()->GetMainWindow());

	GBSHttpClient::getInstance()->unRegisterHandler(this);
	delete ui;

}

//void GBSBizLivePusher::OnEvent(enum obs_frontend_event event)
//{
//	//OBSBasic *main = reinterpret_cast<OBSBasic *>(App()->GetMainWindow());
//	//main->OnEvent(event);
//	
//}

void GBSBizLivePusher::StartStreaming(std::string server, std::string key)
{
	service = obs_service_create("rtmp_custom", "default_service", nullptr,
				     nullptr);
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
			config_get_bool(App()->GetUserConfig(), "BasicWindow",
					"RecordWhenStreaming");


		bool replayBufferWhileStreaming =
			config_get_bool(App()->GetUserConfig(), "BasicWindow",
					"ReplayBufferWhileStreaming");
		if (replayBufferWhileStreaming)
			StartReplayBuffer();

#ifdef YOUTUBE_ENABLED
		if (!autoStartBroadcast)
			OBSBasic::ShowYouTubeAutoStartWarning();
#endif
	};

	setupStreamingGuard =
		outputHandler->SetupStreaming(service, finish_stream_setup);
}



void GBSBizLivePusher::StartReplayBuffer()
{
	if (!outputHandler || !outputHandler->replayBuffer)
		return;
	if (outputHandler->ReplayBufferActive())
		return;

}

static inline QColor color_from_int(long long val)
{
	return QColor(val & 0xff, (val >> 8) & 0xff, (val >> 16) & 0xff, (val >> 24) & 0xff);
}


QColor GBSBizLivePusher::GetSelectionColor() const
{
	if (config_get_bool(App()->GetUserConfig(), "Accessibility", "OverrideColors")) {
		return color_from_int(config_get_int(App()->GetUserConfig(), "Accessibility", "SelectRed"));
	} else {
		return QColor::fromRgb(255, 0, 0);
	}
}

QColor GBSBizLivePusher::GetCropColor() const
{
	if (config_get_bool(App()->GetUserConfig(), "Accessibility", "OverrideColors")) {
		return color_from_int(config_get_int(App()->GetUserConfig(), "Accessibility", "SelectGreen"));
	} else {
		return QColor::fromRgb(0, 255, 0);
	}
}

QColor GBSBizLivePusher::GetHoverColor() const
{
	if (config_get_bool(App()->GetUserConfig(), "Accessibility", "OverrideColors")) {
		return color_from_int(config_get_int(App()->GetUserConfig(), "Accessibility", "SelectBlue"));
	} else {
		return QColor::fromRgb(0, 127, 255);
	}
}

OBSScene GBSBizLivePusher::GetCurrentScene()
{
	return currentScene.load();
}
