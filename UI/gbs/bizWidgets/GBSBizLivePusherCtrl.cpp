#include "GBSBizLivePusherCtrl.h"
#include "ui_GBSBizLivePusherCtrl.h"
#include "qscrollbar.h"
#include <QTimer>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include "qt-wrappers.hpp"

#include "../common/DanmakuWidget.h"

#include "window-basic-main.hpp"
#include "display-helpers.hpp"
#include "gbs/common/QBizLogger.h"
#include "gbs/common/DoubleTextButton.h"
#include "gbs/GBSMainCollector.h"
#include "gbs/common/QIniFile.h"
#include "gbs/GBSDanmaType.h"
#include "gbs/common/QToast.h"
#include "gbs/bizWidgets/GBSMsgDialog.h"
#include "gbs/common/SystemUtils.h"


/**
QFrame边框的样式
background-color:#F9F9F9;
border: 1px solid  #FFFFFF;
border-radius: 16px;



 */

/*
*
btnStartLive样式
background-image: url(:gbs/images/gbs/biz/gbs-start-living.png);

background: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, 
stop:0 #000000, stop:0.525 #434343, stop:1 #000000); 
border-radius: 20px;
*/
#include "gbs/common/GBSHttpClient.h"
#include "qt-display.hpp"

#include "../GBSToolKits.h"



class PCMReader {
public:
	PCMReader() : file_(nullptr), frame_size_(480) {} // 10ms的数据为一帧

	bool Open(const char *filename)
	{
		file_ = fopen(filename, "rb");
		return file_ != nullptr;
	}

	void Close()
	{
		if (file_) {
			fclose(file_);
			file_ = nullptr;
		}
	}

	// 读取一帧数据
	// frame_buffer: 输出缓冲区
	// return: 实际读取的样本数（单个通道），失败返回0
	size_t ReadFrame(std::vector<int> &frame_buffer)
	{
		if (!file_)
			return 0;

		// 一帧的总样本数 = 每通道样本数 * 通道数
		const size_t total_samples = frame_size_ * 2; // 2表示双通道
		frame_buffer.resize(total_samples);

		// 读取一帧数据
		size_t read_samples = fread(frame_buffer.data(), sizeof(int), total_samples, file_);

		if (read_samples == 0) {
			// 文件结束或读取错误
			if (feof(file_)) {
				// 可选：循环播放
				fseek(file_, 0, SEEK_SET);
				read_samples = fread(frame_buffer.data(), sizeof(int), total_samples, file_);
			}
		}

		return read_samples / 2; // 返回单通道的样本数
	}

	~PCMReader() { Close(); }

private:
	FILE *file_;
	const size_t frame_size_; // 每帧的样本数（单通道）
};



GBSBizLivePusherCtrl::GBSBizLivePusherCtrl(QWidget *parent) : QWidget(parent), ui(new Ui::GBSBizLivePusherCtrl)
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
	//ui->tabWidget->setStyleSheet("QTabWidget::pane {"
	//			     "    border: none;" // 移除tab pane的边框
	//			     "}"
	//			     "QTabBar::tab {"
	//			     "    background: none;"   // tab的背景颜色
	//			     "    padding: 10px;"      // tab内容的填充
	//			     "    margin-right: 10px;" // 调整tab之间的水平间距
	//			     "    border: none;"       // 移除tab的边框
	//			     "    color: #01C667;"
	//			     "    font-size:16px;"
	//			     "}"
	//			     "QTabBar::tab:first {"
	//			     "    margin-left: 60px;" // 调整第一个tab项的左外边距
	//			     "}"
	//			     "QTabBar::tab:selected {"
	//			     "    background: #C0C0C0;" // 选中tab的背景颜色
	//			     "    border: none;"        // 选中时也不显示边框
	//			     "    border-radius: 16px"
	//			     "}"
	//			     "QTabBar {"
	//			     "    qproperty-alignment: AlignCenter;" // 设置tab项居中对齐
	//			     "}");

	ui->tabWidget->setStyleSheet("QTabWidget::pane {"
			"    border: none;" // 移除tab pane的边框
			"}");               // 清空 QTabWidget 的样式表

	ui->tabWidget->setStyleSheet("QTabBar::tab {"
					       "    color: #78828A; "     // 默认字体颜色
					       "    background: none; "   // 默认背景颜色
					       "    padding: 40px; "      // 内边距
					       "    padding: 4px; "       // 内边距
					       "    border-radius: 5px; " // 圆角
					       "    font-size: 16px;"     // 设置字体大小为16像素

					       "}"
					       "QTabBar::tab:selected {"
					       "    color: #00C566; "   // 选中字体颜色
					       "    background: none; " // 选中背景颜色
					       "    padding: 4px; "     // 内边距
					       "}"
					       "QTabBar::tab:first {"
					       "    margin-left: 100px;" // 调整第一个tab项的左外边距
					       "}"
					       "QTabWidget::pane { border: 0; }"
					       "QTabBar::tab { border: none; }"
					       "QTabWidget::tab-bar { border: none; }"
	);


	connect(ui->tabWidget, &QTabWidget::currentChanged, this, &GBSBizLivePusherCtrl::onTabChanged);

	QWidget *currentWidget = ui->tabWidget->currentWidget();

	// ScrollArea 包裹 widgets 的容器
	danmakuscrollArea = new QScrollArea(currentWidget);
	danmakuscrollArea->setWidget(containerWidget);
	danmakuscrollArea->setWidgetResizable(true);
	danmakuscrollArea->setStyleSheet("QScrollArea { border: none; }"
					 "QScrollBar:vertical { width: 10px; }" // 调整垂直滚动条的宽度
					 "QScrollBar::handle:vertical { background: #CCCCCC; }" // 滚动条手柄颜色
	);

	// 限制ScrollArea尺寸，防止几何超出问题
	danmakuscrollArea->setMinimumSize(535, 878);
	danmakuscrollArea->setMaximumHeight(3000);

	//QVBoxLayout *outerLayout = new QVBoxLayout(this);
	//outerLayout->addWidget(danmakuscrollArea);
	

	ui->verticalLayout->addWidget(danmakuscrollArea);
	//ui->verticalLayout->addLayout(outerLayout);

	ui->btnAgainstRule->setStyleSheet("border-image:url(:gbs/images/gbs/biz/gbs-widget-docking.png)");

	

	ui->wgtPreview->setStyleSheet("QWidget {"
				      "   background: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, "
				      "       stop:0 #000000, stop:0.525 #434343, stop:1 #000000);" // 渐变背景
				      "   border-radius: 20px;"                                     // 圆角20px
				      "   color: white;"                                            // 文本颜色
				      "   font-size: 16px;"                                         // 字体大小
				      "   padding: 10px;"                                           // 内边距
				      "}");
	ui->wgtPreview->SetLocked(true);

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
		OBSBasic *main = reinterpret_cast<OBSBasic *>(App()->GetMainWindow());
		cameraSource = main->addCameraSource();
		QLogD("Add the Camera source automaically.");
		
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
	connect(ui->cbxCamera, &QComboBox::currentIndexChanged, this, &GBSBizLivePusherCtrl::onComboBoxIndexChanged);

	auto addDisplay = [this](OBSQTDisplay *window) {
		OBSBasic *main = reinterpret_cast<OBSBasic *>(App()->GetMainWindow());
		obs_display_add_draw_callback(window->GetDisplay(), GBSBizLivePusherCtrl::RenderMain, this);

		struct obs_video_info ovi;
		if (obs_get_video_info(&ovi))
			ResizePreview(ovi.base_width, ovi.base_height);
	};

	ui->wgtPreview->Init();
	connect(ui->wgtPreview, &OBSQTDisplay::DisplayCreated, addDisplay);
	connect(ui->btnStartLive, &QPushButton::clicked, this, &GBSBizLivePusherCtrl::onStartRtmpPush);
	ui->btnStartLive->pressed("开播", "已关播", false);


	OBSBasic *main = reinterpret_cast<OBSBasic *>(App()->GetMainWindow());
	outputHandler.reset();
	outputHandler.reset(CreateSimpleOutputHandler(main));

	GBSHttpClient::getInstance()->registerHandler(this);

	ui->btnAll->setCheckable(true);  // 允许按钮被选中
	ui->btnFans->setCheckable(true); // 允许按钮被选中
	ui->btnTips->setCheckable(true); // 允许按钮被选中
	ui->btnF2F->setCheckable(true);  // 允许按钮被选中

	  // 初始化 QThreadStorage 对象
	thlWhoIsDanmukus.setLocalData(QList<DanmaItem>());
	thlAllDanmakus.setLocalData(QList<DanmaItem>());
	thlGiftDanmakus.setLocalData(QList<DanmaItem>());
	thlLikeDanmakus.setLocalData(QList<DanmaItem>());


	connect(ui->btnAll, &QPushButton::toggled, this, &GBSBizLivePusherCtrl::updateStyle);
	connect(ui->btnFans, &QPushButton::toggled, this, &GBSBizLivePusherCtrl::updateStyle);
	connect(ui->btnTips, &QPushButton::toggled, this, &GBSBizLivePusherCtrl::updateStyle);
	connect(ui->btnF2F, &QPushButton::toggled, this, &GBSBizLivePusherCtrl::updateStyle);
	connect(ui->btnAll, &QPushButton::clicked, this, [this]() {

		mDanmakuType = DANITEM_TYPE_ALL;
		QList<DanmaItem> danmItemAllList = thlAllDanmakus.localData();
		{
			while (QLayoutItem *item = danmaKuAreaLayout->takeAt(0)) {
				if (item->widget()) {
					item->widget()->deleteLater();
				}
				delete item;
			}
			for (DanmaItem item : danmItemAllList) {
				emit signalDanmakuReceived(item.deviceName, item.iamgePath, item.danmaku, item.type);
			}
		}
		});
	connect(ui->btnFans, &QPushButton::clicked, this, [this]() {
		mDanmakuType = DANITEM_TYPE_WHOIS;
		QList<DanmaItem> &danmItemWhoisList = thlWhoIsDanmukus.localData();

		{
			while (QLayoutItem *item = danmaKuAreaLayout->takeAt(0)) {
				if (item->widget()) {
					item->widget()->deleteLater();
				}
				delete item;

			}
			for (DanmaItem item : danmItemWhoisList) {
				emit signalDanmakuReceived(item.deviceName, item.iamgePath, item.danmaku, item.type);
			}
		}

	});
	connect(ui->btnTips, &QPushButton::clicked, this, [this]() {
		mDanmakuType = DANITEM_TYPE_GIFT;

		QList<DanmaItem> &danmItemGiftList = thlGiftDanmakus.localData();
		{
			while (QLayoutItem *item = danmaKuAreaLayout->takeAt(0)) {
				if (item->widget()) {
					item->widget()->deleteLater();
				}
				delete item;
			}
			for (DanmaItem item : danmItemGiftList) {
				emit signalDanmakuReceived(item.deviceName, item.iamgePath, item.danmaku, item.type);
			}
		}
	
	});
	connect(ui->btnF2F, &QPushButton::clicked, this, [this]() {
		mDanmakuType = DANITEM_TYPE_CHAT;
		QList<DanmaItem> &danmItemChatList = thlChatDanmakus.localData();
		{
			while (QLayoutItem *item = danmaKuAreaLayout->takeAt(0)) {
				if (item->widget()) {
					item->widget()->deleteLater();
				}
				delete item;
			}
			for (DanmaItem item : danmItemChatList) {
				emit signalDanmakuReceived(item.deviceName, item.iamgePath, item.danmaku, item.type);
			}
		}
		});
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

	mHeartBeatTimer = new QTimer(this);
	connect(mHeartBeatTimer, &QTimer::timeout, this, &GBSBizLivePusherCtrl::onTimeout);
	mHeartBeatTimer->setInterval(5000*60);
	mHeartBeatTimer->start();

	GBSLiveAccountInfo account = GBSMainCollector::getInstance()->getAccountInfo();
	mWssKeepaliveId = std::to_string(account.getId());

	int userId = account.getUserId();
	mWebSocketClient = WebSocketClient::Create();
	mWebSocketClient->setName("DamakuReciver");
	if (!mWebSocketClient->IsRunnig()) {
		QLogD("Start Weboscket userid %d", userId);
		std::string url = GBSMainCollector::getInstance()->getBaseWebSocketV2();
		std::string wssUrl = url + "/sendBarrageToTvAnchor/" + std::to_string(account.getId());
		mWebSocketClient->Start(wssUrl);
	}
	mWebSocketClient->RegisterHandler(this);
	connect(this, &GBSBizLivePusherCtrl::signalDanmakuReceived, this, &GBSBizLivePusherCtrl::addNewWidget);

	//audioWriter = GBSAudioWriter::Create();
	//static PCMReader pcmReader;
	//pcmReader.Open("D:\\GBYX\\github\\obs-studio\\build_x64\\rundir\\Debug\\bin\\64bit\\audio_data.pcm");
	//std::thread writerThread([this]() {
	//	//while (1) {
	//	//	std::vector<int> frame_buffer;
	//	//	pcmReader.ReadFrame(frame_buffer);
	//	//	audioWriter->write((const uint8_t *)(&(frame_buffer[0])), 3840);
	//	//	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	//	//}
	//});
	//writerThread.detach();

	mWssTimer = new QTimer(this);
	connect(mWssTimer, &QTimer::timeout, this, &GBSBizLivePusherCtrl::onWssKeepAlive);
	if (!mWssTimer->isActive()) {
		mWssTimer->start();
		mWssTimer->setInterval(10000);
	}
}

void GBSBizLivePusherCtrl::onWssKeepAlive() {
	if (mWebSocketClient && mWssRunning) {
		std::string target = "ping_" + mWssKeepaliveId;
		mWebSocketClient->Send(target);
	}
}
void GBSBizLivePusherCtrl::onTabChanged(int index) {



}
void GBSBizLivePusherCtrl::onMessage(std::string msg){
	try {
		auto jsonObject = nlohmann::json::parse(msg);
		if (jsonObject.is_object()) {
			processDanmaItem(jsonObject);
		} else if (jsonObject.is_array()) {
			int size = (int)jsonObject.size();
			for (int i = 0; i < size; i++) {
				processDanmaItem(jsonObject[i]);
			}
		}
	} catch (const nlohmann::json::parse_error &e) {
		qDebug() << "JSON 解析错误: " << msg << " Execption: " << e.what();
	} catch (const std::exception &e) {
		qDebug() << "标准异常: " << msg << " Execption: " << e.what();
	}
}
void GBSBizLivePusherCtrl::onOpen() {
	onFailedProcessing = false;
	qDebug() << "onOpen";
}

void GBSBizLivePusherCtrl::onFail() {
	qDebug() << "onFail";
	Sleep(5 * 1000);
	if (!onFailedProcessing) {
		onFailedProcessing = true;
		QMetaObject::invokeMethod(
			this,
			[this]() {
				mWebSocketClient->Stop();
				GBSLiveAccountInfo account = GBSMainCollector::getInstance()->getAccountInfo();
				int userId = account.getUserId();
				mWebSocketClient = WebSocketClient::Create();
				mWebSocketClient->setName("DamakuReciver");
				if (!mWebSocketClient->IsRunnig()) {
					QLogD("Start Weboscket userid %d", userId);
					std::string url = GBSMainCollector::getInstance()->getBaseWebSocketV2();
					std::string wssUrl = url + "/sendBarrageToTvAnchor/" + std::to_string(account.getId());
					mWebSocketClient->Start(wssUrl);
				}
				mWebSocketClient->RegisterHandler(this);
			},
			Qt::QueuedConnection);
	}

}

void GBSBizLivePusherCtrl::onClose() {
	qDebug() << "onClose";
}

void GBSBizLivePusherCtrl::onTimeout()
{
	if (startLive) {
		GBSHttpClient::getInstance()->sendHeartbeatTimeV2(mLiveAccountId);
	}
}

void GBSBizLivePusherCtrl::onRtmpPushError(std::string errMsg) {
	QMetaObject::invokeMethod(this, [errMsg, this]() {
		if (!errMsg.empty()) {
			QWidget *widget = new QWidget;
			QVBoxLayout *layout = new QVBoxLayout(widget);
			QLabel *label = new QLabel(QString::fromStdString(errMsg));
			label->setWordWrap(true);
			label->setStyleSheet("QLabel {"
					    "  font-size: 24px;"
					    "}");
			QSpacerItem *spacer0 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Minimum);
			layout->addSpacerItem(spacer0);
			layout->setAlignment(Qt::AlignHCenter); // 整体内容居中
			layout->addWidget(label);
			QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
			layout->addSpacerItem(spacer);
			GBSMsgDialog *dialog = new GBSMsgDialog("提示", layout, this);
			dialog->exec();
		}
		ui->btnStartLive->click();
	});

}

void GBSBizLivePusherCtrl::updateStyle(bool checked) {
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

bool GBSBizLivePusherCtrl::FindCameraSource(void *param, obs_source_t *source)
{
	GBSBizLivePusherCtrl *self = reinterpret_cast<GBSBizLivePusherCtrl *>(param);
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



void GBSBizLivePusherCtrl::onStartRtmpPush(bool checked) {
	
	DoubleTextButton *button = qobject_cast<DoubleTextButton *>(sender());
	startLive = !startLive;

	if (button) {
		GBSLiveAccountInfo account = GBSMainCollector::getInstance()->getAccountInfo();
		if (startLive) {
			//QTimer::singleShot(8000, this, &GBSBizLivePusherCtrl::streamCheck);
			button->pressed("关播", "直播中", true);
			QLogE("GBSBizLivePusherCtrl 开播 %d", mLiveAccountId);
			GBSHttpClient::getInstance()->createSrsStreamV2(1);
			GBSMainCollector::getInstance()->setLiving(true);
		} else {
			button->pressed("开播", "已关播", false);
			QLogE("GBSBizLivePusherCtrl 关播 %d", mLiveAccountId);
			GBSHttpClient::getInstance()->closeSrsStreamLogV2(mLiveAccountId);
			GBSMainCollector::getInstance()->setLiving(false);
		}
	}

}

void GBSBizLivePusherCtrl::streamCheck(){
	OBSBasic *main = reinterpret_cast<OBSBasic *>(App()->GetMainWindow());
	obs_service_t* service = main->GetService();
	if (service) {
		
		OBSSourceAutoRelease source = obs_get_source_by_name("RTMP 矩阵地址");
		obs_media_state state = obs_source_media_get_state(source);
		if (state == OBS_MEDIA_STATE_ENDED) {
			ui->btnStartLive->click();
			QWidget *widget = new QWidget;
			QVBoxLayout *layout = new QVBoxLayout(widget);
			QLabel *label = new QLabel(QString::fromStdString("媒体服务器异常"));
			label->setWordWrap(true);
			label->setStyleSheet("QLabel {"
					     "  font-size: 24px;"
					     "}");
			QSpacerItem *spacer0 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Minimum);
			layout->addSpacerItem(spacer0);
			layout->setAlignment(Qt::AlignHCenter); // 整体内容居中
			layout->addWidget(label);
			QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
			layout->addSpacerItem(spacer);
			GBSMsgDialog *dialog = new GBSMsgDialog("提示", layout, this);
			dialog->exec();
		}


	}
}


void GBSBizLivePusherCtrl::ResizePreview(uint32_t cx, uint32_t cy)
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


void GBSBizLivePusherCtrl::RenderMain(void *data, uint32_t, uint32_t)
{
	GS_DEBUG_MARKER_BEGIN(GS_DEBUG_COLOR_DEFAULT, "RenderMain");

	GBSBizLivePusherCtrl *window = static_cast<GBSBizLivePusherCtrl *>(data);
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




void GBSBizLivePusherCtrl::onComboBoxIndexChanged(int index) {
	if (currentCameraIndex != index) {
		if (cameraSource) {
			GBSToolKits::getInstance()->StopPreview(cameraSource);
			GBSToolKits::getInstance()->StartPreview(cameraSource,
								 index);
			currentCameraIndex = index;
		}
	}
}

//void GBSBizLivePusherCtrl::addNewWidget() {
//	++danmaindex;
//	QString text = "这是动态设置的第" + QString::number(danmaindex) + "行文本";
//	addNewWidget2(text, ":gbs/images/gbs/biz/gbs-logo-douyin.png", "文本会自动换行以适应窗口宽度。我这里就需要看到你换行,这是动态设置的第二行文本。文本会自动换行以适应窗口宽度。我这里就需要看到你换行");
//
//}


void GBSBizLivePusherCtrl::addNewWidget(const QString &atext,
				    const QString &aimagePath,
				    const QString &atext2,
					const QString &atype)
{
	QString text = atext;
	QString imagePath = aimagePath;
	QString text2 = atext2;
	QString type = atype;
	DanmaItem item{QTime::currentTime(), text, imagePath, text2, type};
	int danmakuType = DANITEM_TYPE_ALL;
	QList<DanmaItem> danmItemWhoisList = thlWhoIsDanmukus.localData();
	QList<DanmaItem> danmItemAllList = thlAllDanmakus.localData();
	QList<DanmaItem> danmItemGiftList = thlGiftDanmakus.localData();
	QList<DanmaItem> danmItemLikeList = thlLikeDanmakus.localData();
	QList<DanmaItem> danmItemChatList = thlChatDanmakus.localData();

	if (type == "MemberMessage") {
		danmItemWhoisList.push_back(item);
		if (danmItemWhoisList.size() > 300) {
			danmItemWhoisList.pop_front();
		}
		danmakuType = DANITEM_TYPE_WHOIS;
	} else if (type == "ChatMessage") {
		danmItemChatList.push_back(item);
		if (danmItemChatList.size() > 300) {
			danmItemChatList.pop_front();
		}
		danmakuType = DANITEM_TYPE_CHAT;
	} else if (type == "GiftMessage") {
		danmItemGiftList.push_back(item);
		if (danmItemGiftList.size() > 300) {
			danmItemGiftList.pop_front();
		}
		danmakuType = DANITEM_TYPE_GIFT;
	} else if (type == "SocialMessage") {
	} else if (type == "LikeMessage") {
		danmItemLikeList.push_back(item);
		if (danmItemLikeList.size() > 300) {
			danmItemLikeList.pop_front();
		}
		danmakuType = DANITEM_TYPE_LIKE;
	}
	if (danmItemAllList.size() > 300) {
		danmItemAllList.pop_front();
	}
	danmItemAllList.push_back(item);

	if (mDanmakuType != DANITEM_TYPE_ALL) {
		if (mDanmakuType != danmakuType) {
			return;
		}
	}
	

	// 创建新 widget
	
	DanmakuWidget *newWidget = new DanmakuWidget(ui->tabWidget->currentWidget());
	newWidget->setFirstRowContent(text, imagePath);
	newWidget->setSecondRowContent(text2);

	// 添加到布局
	danmaKuAreaLayout->setEnabled(false);

	// 插入到布局的最底部
	int count = danmaKuAreaLayout->count();
	//danmaKuAreaLayout->insertWidget(danmaKuAreaLayout->count(), newWidget);  // 最新的 widget 添加到布局的最后
	danmaKuAreaLayout->addWidget(newWidget);
	qDebug() << "danmaKuAreaLayout count " << danmaKuAreaLayout->count() << " count " << count;

	// 滚动到最底部显示最新添加的widget
	QScrollBar *vScrollBar = danmakuscrollArea->verticalScrollBar();
	vScrollBar->blockSignals(true); // 避免不必要的信号触发
	vScrollBar->setValue(vScrollBar->maximum());
	vScrollBar->blockSignals(false);
	danmaKuAreaLayout->setEnabled(true);

	// 检查是否超过 300 个 widget
	if (danmaKuAreaLayout->count() > 300) {
		// 移除最早的 widget
		QLayoutItem *oldestItem = danmaKuAreaLayout->takeAt(0);
		if (oldestItem->widget()) {
			delete oldestItem->widget();
		}
		delete oldestItem;
	}
	
}

void GBSBizLivePusherCtrl::onRtmpPushUrl(std::string url, int liveAccountId) {
	
	qDebug() << "GBSBizLivePusherCtrl URL: " << url;
	QLogD("Rtmp url %s", url.c_str());
	if (url.empty()) {
		QLogE("Cannot retrieve the rtmp push url ");
		return;
	}
	qPushUrl = QString::fromStdString(url);
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

		GBSMainCollector::getInstance()->updateLivePushUrl(url);
		std::unique_ptr<IniSettings> iniFile = std::make_unique<IniSettings>("gbs.ini");
		iniFile->setValue("livePusher", "url", qPushUrl);
		iniFile->setValue("livePusher", "running", true);
	}
	mLiveAccountId = liveAccountId;
}


void GBSBizLivePusherCtrl::onPushRtmpClosed() {
	StopStreaming();
}


void GBSBizLivePusherCtrl::onPullRtmpUrl(std::string url) {
}

void GBSBizLivePusherCtrl::onUserInfo(const GBSUserInfo *info) {}

void GBSBizLivePusherCtrl::onUserFileDownLoad(const std::string &path, int type) {
	qDebug() << "GBSBizLivePusherCtrl onUserFileDownLoad: " << path;
}

void GBSBizLivePusherCtrl::onRoomInfos(std::list<GBSRoomInfo>& infos) {
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
		//connect(selectIDSDialog, &SelectedIDSDialog::dataSubmitted, this,
		//		&GBSBizLivePusherCtrl::dialogSubmitted);	
	}, Qt::QueuedConnection);

}

void GBSBizLivePusherCtrl::dialogSubmitted(const QString &data) {
	std::string ids = data.toStdString();
	GBSHttpClient::getInstance()->upRemoteLiveRoomStateTask(ids);
}
void GBSBizLivePusherCtrl::onRoomInfo(GBSRoomInfo *info)
{
	
	int32_t userId = info->getUserId();
	qUserId = QString("%1").arg(userId);
	qDebug() << "GBSBizLivePusherCtrl onRoomInfo: " << qUserId;
}

GBSBizLivePusherCtrl::~GBSBizLivePusherCtrl()
{
	mWssTimer->stop();
	mWebSocketClient->UnRegisterHandler(this);
	mWebSocketClient->Stop();

	mHeartBeatTimer->stop();
	service = nullptr;
	outputHandler.reset();
	obs_display_remove_draw_callback(ui->wgtPreview->GetDisplay(),
					 GBSBizLivePusherCtrl::RenderMain,
					 this);

	
	if (cameraSource) {
		//GBSToolKits::getInstance()->StopPreview(cameraSource);
	}
	
	OBSBasic *main = reinterpret_cast<OBSBasic *>(App()->GetMainWindow());

	GBSHttpClient::getInstance()->unRegisterHandler(this);
	delete ui;

}

//void GBSBizLivePusherCtrl::OnEvent(enum obs_frontend_event event)
//{
//	//OBSBasic *main = reinterpret_cast<OBSBasic *>(App()->GetMainWindow());
//	//main->OnEvent(event);
//	
//}

void GBSBizLivePusherCtrl::StartStreaming(std::string server, std::string key)
{
	OBSBasic *main = reinterpret_cast<OBSBasic *>(App()->GetMainWindow());
	main->StartGBSStreaming(server, key);
	return;
	//	service = obs_service_create("rtmp_custom", "default_service", nullptr,
//				     nullptr);
//	if (!service)
//		return;
//	obs_service_release(service);
//
//
//	
//	
//	obs_data_t *settings1 = obs_service_get_settings(service);
//	obs_data_set_string(settings1, "server",
//			    server.c_str()); // 设置 RTMP 服务器地址
//	obs_data_set_string(settings1, "key",
//			    key.c_str()); // 设置推流码 (Stream Key)
//
//	obs_service_update(service, settings1);
//	obs_data_release(settings1);
//
//	if (outputHandler->StreamingActive())
//		return;
//	
//
//
//
//	auto finish_stream_setup = [&](bool setupStreamingResult) {
//		if (!setupStreamingResult) {
//			return;
//		}
//		//OnEvent(OBS_FRONTEND_EVENT_STREAMING_STARTING);
//
//		if (!outputHandler->StartStreaming(service)) {
//			return;
//		}
//
//
//		bool recordWhenStreaming =
//			config_get_bool(App()->GetUserConfig(), "BasicWindow",
//					"RecordWhenStreaming");
//
//
//		bool replayBufferWhileStreaming =
//			config_get_bool(App()->GetUserConfig(), "BasicWindow",
//					"ReplayBufferWhileStreaming");
//		if (replayBufferWhileStreaming)
//			StartReplayBuffer();
//
//#ifdef YOUTUBE_ENABLED
//		if (!autoStartBroadcast)
//			OBSBasic::ShowYouTubeAutoStartWarning();
//#endif
//	};
//
//	setupStreamingGuard =
//		outputHandler->SetupStreaming(service, finish_stream_setup);
}


void GBSBizLivePusherCtrl::StopStreaming() {
	OBSBasic *main = OBSBasic::Get();
	main->StopGBSStreaming();
	return;
	//OBSBasic *main = OBSBasic::Get();
	//main->SaveProject();

	//if (outputHandler->StreamingActive())
	//	outputHandler->StopStreaming(true);
	//


	//bool recordWhenStreaming = config_get_bool(App()->GetUserConfig(), "BasicWindow", "RecordWhenStreaming");
	//bool keepRecordingWhenStreamStops =
	//	config_get_bool(App()->GetUserConfig(), "BasicWindow", "KeepRecordingWhenStreamStops");
	//if (recordWhenStreaming && !keepRecordingWhenStreamStops)
	//	main->StopRecording();

	//bool replayBufferWhileStreaming =
	//	config_get_bool(App()->GetUserConfig(), "BasicWindow", "ReplayBufferWhileStreaming");
	//bool keepReplayBufferStreamStops =
	//	config_get_bool(App()->GetUserConfig(), "BasicWindow", "KeepReplayBufferStreamStops");
	//if (replayBufferWhileStreaming && !keepReplayBufferStreamStops)
	//	main->StopReplayBuffer();
}



void GBSBizLivePusherCtrl::StartReplayBuffer()
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


QColor GBSBizLivePusherCtrl::GetSelectionColor() const
{
	if (config_get_bool(App()->GetUserConfig(), "Accessibility", "OverrideColors")) {
		return color_from_int(config_get_int(App()->GetUserConfig(), "Accessibility", "SelectRed"));
	} else {
		return QColor::fromRgb(255, 0, 0);
	}
}

QColor GBSBizLivePusherCtrl::GetCropColor() const
{
	if (config_get_bool(App()->GetUserConfig(), "Accessibility", "OverrideColors")) {
		return color_from_int(config_get_int(App()->GetUserConfig(), "Accessibility", "SelectGreen"));
	} else {
		return QColor::fromRgb(0, 255, 0);
	}
}

QColor GBSBizLivePusherCtrl::GetHoverColor() const
{
	if (config_get_bool(App()->GetUserConfig(), "Accessibility", "OverrideColors")) {
		return color_from_int(config_get_int(App()->GetUserConfig(), "Accessibility", "SelectBlue"));
	} else {
		return QColor::fromRgb(0, 127, 255);
	}
}

OBSScene GBSBizLivePusherCtrl::GetCurrentScene()
{
	return currentScene.load();
}



void GBSBizLivePusherCtrl::processDanmaItem(const nlohmann::json jsonObject)
{
	std::string jsonString = jsonObject.dump();
	std::string platform = jsonObject["platform"].get<std::string>();
	std::string liveId = jsonObject["liveId"].get<std::string>();             //类似于D01，K01之类
	std::string liveDeviceId = jsonObject["liveDeviceId"].get<std::string>(); 
	std::string deviceName = jsonObject["deviceName"].get<std::string>(); 

	
	if (deviceName.empty()) {
		return;
	}
	QString qUniqueName = QString::fromStdString(deviceName);
	if (danmaPlatIconString.isEmpty()) {

		if (platform == "DY") {
			danmaPlatIconString = ":gbs/images/gbs/biz/gbs-logo-douyin.png";
		} else if (platform == "KS") {
			danmaPlatIconString = ":gbs/images/gbs/biz/gbs-logo-kuai.png";
		} else if (platform == "SPH") {
			danmaPlatIconString = ":gbs/images/gbs/biz/gbs-logo-wechat.png";
		} else if (platform == "TK") {
			danmaPlatIconString = ":gbs/images/gbs/biz/gbs-logo-tiktok.png";
		} else if (platform == "BILI") {
			danmaPlatIconString = ":gbs/images/gbs/biz/gbs-logo-bilibili.png";
		} else if (platform == "PDD") {
			danmaPlatIconString = ":gbs/images/gbs/biz/gbs-logo-pdd.png";
		} else if (platform == "FB") {
			danmaPlatIconString = ":gbs/images/gbs/biz/gbs-logo-facebook.png";
		}
	}
	if (danmaPlatIconString.isEmpty()) {
		QLogE("No platform about danmaku, ProcessDanmaItem failed.");
		return;
	}

	std::string type = jsonObject["type"].get<std::string>();
	qDebug() << "damaku type " << type;
	if (type == "MemberMessage") {
		auto danma = std::make_shared<DammaMemberMSG>();
		danma->type = "MemberMessage";
		danma->name = jsonObject["name"].get<std::string>();
		danma->head_image = jsonObject["head_image"].get<std::string>();

		danma->content = jsonObject["content"].get<std::string>();
		danma->msgType = 1;

		QString danmaText = QString::fromStdString(danma->name) + ":" + QString::fromStdString(danma->content);

		emit signalDanmakuReceived(qUniqueName, danmaPlatIconString, danmaText, QString::fromStdString(type));

	} else if (type == "ChatMessage") {
		auto danma = std::make_shared<DanmaChatMessage>();
		danma->type = "ChatMessage";
		danma->name = jsonObject["name"].get<std::string>();
		danma->head_image = jsonObject["head_image"].get<std::string>();
		danma->content = jsonObject["content"].get<std::string>();
		danma->msgType = 2;
		QString danmaText = QString::fromStdString(danma->name) + ":" + QString::fromStdString(danma->content);
		emit signalDanmakuReceived(qUniqueName, danmaPlatIconString, danmaText, QString::fromStdString(type));

	} else if (type == "GiftMessage") {
		auto danma = std::make_shared<DanmaGiftMessage>();
		danma->type = "GiftMessage";
		danma->name = jsonObject["name"].get<std::string>();
		danma->head_image = jsonObject["head_image"].get<std::string>();
		danma->content = jsonObject["content"].get<std::string>();
		danma->gift_name = jsonObject["gift_name"].get<std::string>();
		danma->gift_count = jsonObject["gift_count"].get<std::string>();
		danma->msgType = 3;
		QString danmaText = QString::fromStdString(danma->name) + ":" + QString::fromStdString(danma->content);
		emit signalDanmakuReceived(qUniqueName, danmaPlatIconString, danmaText, QString::fromStdString(type));

	} else if (type == "SocialMessage") {
		auto danma = std::make_shared<DanmaSocialMessage>();
		danma->type = "SocialMessage";
		danma->name = jsonObject["name"].get<std::string>();
		danma->head_image = jsonObject["head_image"].get<std::string>();
		danma->content = jsonObject["content"].get<std::string>();
		danma->msgType = 4;
		QString danmaText = QString::fromStdString(danma->name) + ":" + QString::fromStdString(danma->content);
		emit signalDanmakuReceived(qUniqueName, danmaPlatIconString, danmaText, QString::fromStdString(type));
	} else if (type == "LikeMessage") {
		auto danma = std::make_shared<DanmaLikeMessage>();
		danma->type = "LikeMessage";
		danma->name = jsonObject["name"].get<std::string>();
		danma->head_image = jsonObject["head_image"].get<std::string>();
		danma->content = jsonObject["content"].get<std::string>();
		danma->count = jsonObject["count"].get<std::string>();
		danma->msgType = 5;
		QString danmaText = QString::fromStdString(danma->name) + ":" + QString::fromStdString(danma->content);
		emit signalDanmakuReceived(qUniqueName, danmaPlatIconString, danmaText, QString::fromStdString(type));
	} else if (type == "RoomMessage") {

		auto danma = std::make_shared<DanmaRoomMessage>();
		danma->content = jsonObject["content"].get<std::string>();
		danma->count = jsonObject["count"].get<std::string>();
		danma->msgType = 6;
		QString danmaText = QString::fromStdString(deviceName) + ":" + QString::fromStdString(danma->content);
		emit signalDanmakuReceived(qUniqueName, danmaPlatIconString, danmaText, QString::fromStdString(type));



	}
}
