#include "GBSBizDeviceInfo.h"
#include "ui_GBSBizDeviceInfo.h"
#include <QComboBox>
#include <QStringList>
#include <QLineEdit>
#include <QSlider>
#include "window-basic-main.hpp"
#include "gbs/common/QIniFile.h"
#include "gbs/common/SystemUtils.h"
#include "gbs/bizWidgets/GBSMsgDialog.h"
#include "gbs/common/QBizLogger.h"

extern "C" {
#include "gbs/common/SystemUtils.h"
}

#define TIMER_INTERVAL 2000
#define REC_TIME_LEFT_INTERVAL 30000

static uint32_t first_encoded = 0xFFFFFFFF;
static uint32_t first_skipped = 0xFFFFFFFF;
static uint32_t first_rendered = 0xFFFFFFFF;
static uint32_t first_lagged = 0xFFFFFFFF;

static QString MakeMissedFramesText(uint32_t total_lagged, uint32_t total_rendered, long double num)
{
	return QString("%1 / %2 (%3%)")
		.arg(QString::number(total_lagged), QString::number(total_rendered), QString::number(num, 'f', 1));
}
static QString ObfuscateString(const QString &input)
{
	int length = input.length();
	if (length <= 8) {
		// 如果字符串长度小于或等于 8，则不进行中间替换
		return input;
	}

	// 提取前四个和后四个字符
	QString start = input.left(4);
	QString end = input.right(4);

	// 中间部分用 '*' 填充
	QString middle = "****";

	return start + middle + end;
}

static QString removeBraces(const QString &input)
{
	QString result = input;

	// 去掉开头的 '{'
	if (!result.isEmpty() && result[0] == '{') {
		result = result.mid(1); // 使用mid()移除第一个字符
	}

	// 去掉结尾的 '}'
	int lastBraceIndex = result.lastIndexOf('}'); // 查找最后一个大括号的位置
	if (lastBraceIndex != -1) {
		// 如果找到大括号，返回大括号之前的子字符串
		result = result.left(lastBraceIndex);
	}

	return result;
}


void GBSBizDeviceInfo::InitializeValues()
{
	video_t *video = obs_get_video();
	first_encoded = video_output_get_total_frames(video);
	first_skipped = video_output_get_skipped_frames(video);
	first_rendered = obs_get_total_frames();
	first_lagged = obs_get_lagged_frames();
}

GBSBizDeviceInfo::GBSBizDeviceInfo(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GBSBizDeviceInfo),
	  timer(this),
	  recTimeLeft(this)
{
	ui->setupUi(this);
    // ui->wdgLocalInfo->setStyleSheet(
    // 	"QWidget {"
    // 	"   background-color: #F5F5F5;"  // 主窗口的背景颜色
    // 	"   border: 2px solid #B0B0B0;"  // 窗口边框
    // 	"   border-radius: 10px;"        // 窗口圆角
    // 	"   padding: 10px;"              // 内边距
    // 	"}");

    ui->comboBox->setStyleSheet("QComboBox {"
				    "    padding-left: 10px;" // 为左侧箭头和文本之间留出足够的空间
				    "    border: 1px solid transparent;" // 去除边框
				    "    background: transparent;"       // 背景透明

				    "}"
				    "QComboBox::drop-down {"
				    "    width: 20px;" // 设置下拉箭头的宽度
				    "    subcontrol-origin: padding;"
				    "    subcontrol-position: left center;" // 将下拉按钮移到左侧
				    "    margin-left: 5px;"                 // 控制箭头与左边框的距离
				    "}"
				    "QComboBox::down-arrow {"
				    "    image: url(:gbs/images/gbs/biz/gbs-right-combox-arrow.png);" // 替换为你的箭头图标
				    "    width: 16px;"                                        // 箭头的宽度
				    "    height: 16px;"                                       // 箭头的高度
				    "    border: none;"
				    "    background: transparent;" // 透明背景
				    "    box-shadow: none;"        // 无阴影
				    "}"
				"QComboBox QAbstractItemView {"
				"border: 1px solid #cccccc;"
				"   selection-background-color: #f0f0f0;"
				"   selection-color: black;"
				"border-radius: 0px;" /* 移除下拉列表的圆角 */

				"}"
    );


    QString greenGroove = "#00C566";
    QString gradientGroove = "qlineargradient(spread:pad, x1:0, y1:0.5, x2:1, y2:0.5, stop:0 #212121, stop:1 #000000)";

    QString greenGrooveStyleSheet = R"(
            "QSlider {"
            "    background: transparent;"
            "}"
            "QSlider::groove:horizontal {"
            "    border: none;"
            "    height: 20px;"
            "    margin: 0px;"
            "    border-radius: 10px;"
            "    background: #00C566;"
            "}"
            "QSlider::handle:horizontal {"
            "    background: white;"
            "    border: 2px solid #000000;"
            "    width: 26px;"
            "    height: 26px;"
            "    margin: -3px 0;"  // 调整滑块位置以适应槽
            "    border-radius: 13px;"
            "}"
            "QSlider::handle:horizontal:pressed {"
            "    width: 8px;" /* 保持滑块宽度一致 */
            "    height: 4px;" /* 保持滑块高度一致 */
            "    margin-left: -2px;" /* 保持外边距一致 */
            "    margin-right: -2px;" /* 保持外边距一致 */
            "}"
            "QSlider::add-page:horizontal {"
            "    background: #EB3F5E;"
            "    border-radius: 2px;"
            "}"
            "QSlider::sub-page:horizontal {"
            "    background: #EB3F5E;"
            "    border-radius: 2px;"
            "}"
    )";


    //ui->horizontalSlider->setRange(0, 1);
    //ui->horizontalSlider->setStyleSheet(
    //    "QSlider {"
    //    "    background: transparent;"
    //    "}"
    //    "QSlider::groove:horizontal {"
    //    "    border: none;"
    //    "    height: 20px;"
    //    "    margin: 0px;"
    //    "    border-radius: 13px;"
    //    "    background: 00C566;"
    //    "}"
    //    "QSlider::handle:horizontal {"
    //    "    background: white;"
    //    "    width: 26px;"
    //    "    height: 26px;"
    //    "    margin: -3px 0;"  // 调整滑块位置以适应槽
    //    "    border-radius: 13px;"
    //    "}"
    //    "QSlider::handle:horizontal:pressed {"
    //    "    width: 8px;" /* 保持滑块宽度一致 */
    //    "    height: 4px;" /* 保持滑块高度一致 */
    //    "    margin-left: -2px;" /* 保持外边距一致 */
    //    "    margin-right: -2px;" /* 保持外边距一致 */
    //    "}"
    //    "QSlider::add-page:horizontal {"
    //    "    background: #00C566;"
    //    "    border-radius: 2px;"
    //    "}"
    //    "QSlider::sub-page:horizontal {"
    //    "    background: #00C566;"
    //    "    border-radius: 2px;"
    //    "}"
    //    );

    connect(ui->horizontalSlider, &QSlider::valueChanged, this, &GBSBizDeviceInfo::onSliderValueChanged);

    
	QObject::connect(&timer, &QTimer::timeout, this, &GBSBizDeviceInfo::Update);
	timer.setInterval(TIMER_INTERVAL);
	timer.start();

	QObject::connect(&recTimeLeft, &QTimer::timeout, this, &GBSBizDeviceInfo::RecordingTimeLeft);
	recTimeLeft.setInterval(REC_TIME_LEFT_INTERVAL);

	QStringList mangedItems;
	mangedItems << "抖音"
		    << "快手"
		    << "哔哩哔哩"
		    << "视频号"
		    << "拼多多"
		    << "TikTok"
		    << "Facobook";
	ui->comboBox->addItems(mangedItems);
	

    connect(ui->btnDevInfo04, &QPushButton::clicked, this, [this]() {
		QWidget *widget = new QWidget();
		QVBoxLayout *layout = new QVBoxLayout(widget);
		QHBoxLayout *bizLayout = new QHBoxLayout();
		bizLayout->setAlignment(Qt::AlignCenter);
		QComboBox *combox = new QComboBox();
		QStringList items;

		items << "抖音"
		      << "快手"
		      << "哔哩哔哩"
		      << "视频号"
		      << "拼多多"
		      << "TikTok"
		      << "Facobook";
		combox->addItems(items);

		//0 抖音
		//1 快手
		//2 哔哩哔哩
		//3 视频号
		//4 拼多多
		//5 TikTok
		//6 Facobook
		std::unique_ptr<IniSettings> iniFile = std::make_unique<IniSettings>("gbs.ini");
		QString str1 = iniFile->value("DeviceData", "Alias.Plat", "DY").toString();
		QString text = "抖音";
		if (str1 == "DY") {
			text = "抖音";
		} else if (str1 == "KS") {
			text = "快手";
		} else if (str1 == "BILI") {
			text = "哔哩哔哩";
		} else if (str1 == "SPH") {
			text = "视频号";
		} else if (str1 == "PDD") {
			text = "拼多多";
		} else if (str1 == "TK") {
			text = "TikTok";
		} else if (str1 == "FB") {
			text = "Facobook";
		}

		combox->setCurrentText(text);

		combox->setFixedSize(90, 40);
		QLineEdit *leName = new QLineEdit();
		leName->setPlaceholderText("别名");
		leName->setFixedSize(90, 40);
		QString str2 = iniFile->value("DeviceData", "Alias.alias", "CZQ").toString();
		leName->setText(str2);
		QLineEdit *leNo = new QLineEdit("");
		leNo->setPlaceholderText("位置信息");
		leNo->setFixedSize(90, 40);
		QString str3 = iniFile->value("DeviceData", "Alias.postion", "0101").toString();
		leNo->setText(str3);
		bizLayout->addWidget(combox);
		bizLayout->addWidget(leName);
		bizLayout->addWidget(leNo);
		layout->addLayout(bizLayout);

		QHBoxLayout *bottomLayout = new QHBoxLayout();
		QPushButton *button = new QPushButton("确认");
		button->setFixedSize(90, 40);
		combox->setStyleSheet(R"(
        QComboBox {
            border: 1px solid #cccccc;
            border-radius: 4px;
            padding: 2px;
            background-color: white;
            color: black;
        }
        QComboBox:editable {
            background: white;
        }
        QComboBox QAbstractItemView {
            border: 1px solid #cccccc;
            selection-background-color: #f0f0f0;
            selection-color: black;
        }
        QComboBox::drop-down {
            border: none;
        }
        QComboBox::down-arrow {
            image: url(:Resources/gbs-arrow-down.png); /* 你可以替换为你自己的下拉箭头图标 */
            width: 15px;
            height: 10px;
        }
    )");
		QLabel *readME = new QLabel(
			"解释说明\n下拉框指示当前工作的平台\n别名可以是一个简称或者其他\n位置信息可以是如0101，代表第1排第1列\n以上所有内容便于运维人员标记使用，本软件不做任何收集或者备份");
		readME->setWordWrap(true);
		readME->setAlignment(Qt::AlignLeft); // 设置文本左对齐
		readME->setStyleSheet("QLabel { text-align: left; }");

		leName->setStyleSheet("QLineEdit {"
				      "background-color: #FFFFFF;" // 背景颜色
				      "border: 1px solid #F9F9F9;" // 边框颜色
				      "border-radius: 5px;"        // 边框圆角
				      "padding: 2px;"              // 内边距
				      "font-size: 14px;"           // 字体大小
				      "color: #333333;"            // 文本颜色
				      "}");
		leNo->setStyleSheet("QLineEdit {"
				    "background-color: #FFFFFF;" // 背景颜色
				    "border: 1px solid #F9F9F9;" // 边框颜色
				    "border-radius: 5px;"        // 边框圆角
				    "padding: 2px;"              // 内边距
				    "font-size: 14px;"           // 字体大小
				    "color: #333333;"            // 文本颜色
				    "}");
		button->setStyleSheet("QPushButton {"
				      "  border: 2px solid #F9F9F9;" // 蓝色边框
				      "  border-radius: 5px;"        // 圆角
				      "}"
				      "QPushButton:hover {"
				      "  border: 2px solid #78828A;"
				      "   background-color: #F9F9F9;"
				      "}"
				      "QPushButton:pressed {"
				      "   background-color: #D1D8DD;" // 按下时背景颜色
				      "   padding-left: 3px;"         // 向左移动 3px
				      "   padding-top: 3px;"          // 向上移动 3px
				      "   background-repeat: no-repeat;"
				      "   background-position: center;"
				      "}");
		bottomLayout->addWidget(button);
		layout->addLayout(bottomLayout);
		layout->addWidget(readME, 0, Qt::AlignCenter); // 居中 QLabel
		layout->addWidget(button, 0, Qt::AlignBottom | Qt::AlignRight);
		GBSMsgDialog *dialog = new GBSMsgDialog("设置", layout, this);
		connect(button, &QPushButton::clicked, dialog, [combox, leName, leNo, dialog, this]() {
			//0 抖音
			//1 快手
			//2 哔哩哔哩
			//3 视频号
			//4 拼多多
			//5 TikTok
			//6 Facobook
			QString str1 = "DY";
			int currentIndex = (combox->currentIndex());
			if (currentIndex == 0) {
				str1 = "DY";
			} else if (currentIndex == 1) {
				str1 = "KS";
			} else if (currentIndex == 2) {
				str1 = "BILI";
			} else if (currentIndex == 3) {
				str1 = "SPH";
			} else if (currentIndex == 4) {
				str1 = "PDD";
			} else if (currentIndex == 5) {
				str1 = "TK";
			} else if (currentIndex == 6) {
				str1 = "FB";
			}
			QString str2 = leName->text();
			QString str3 = leNo->text();

			qDebug() << " str1 " << str1 << " str2 " << str2 << " str3 " << str3;
			std::unique_ptr<IniSettings> iniFile = std::make_unique<IniSettings>("gbs.ini");
			iniFile->setValue("DeviceData", "Alias.Plat", str1);
			iniFile->setValue("DeviceData", "Alias.alias", str2);
			iniFile->setValue("DeviceData", "Alias.postion", str3);
			this->ui->lblDevInfo04->setText("备注编号： " + str1 + "/" + str2 + "/" + str3);
			dialog->accept();
		});
		dialog->exec();
	});

	std::unique_ptr<IniSettings> iniFile = std::make_unique<IniSettings>("gbs.ini");
	QString str1 = iniFile->value("DeviceData", "Alias.Plat", "DY").toString();

	QString str2 = iniFile->value("DeviceData", "Alias.alias", "CZQ").toString();

	QString str3 = iniFile->value("DeviceData", "Alias.postion", "0101").toString();

	ui->lblDevInfo04->setText("备注编号： " + str1 + "/" + str2 + "/" + str3);

	bool enable = iniFile->value("DeviceData", "Remote.enable", false).toBool();

	ui->horizontalSlider->setValue(enable);
	connect(ui->horizontalSlider, &QSlider::valueChanged, this, [this](int value) {
		std::unique_ptr<IniSettings> iniFile = std::make_unique<IniSettings>("gbs.ini");
		iniFile->setValue("DeviceData", "Remote.enable", bool(value));
		});


	
	std::string productId = GetWindowsProductIDFromRegistery();
	std::string deviceId = GetMachineIdFromRegistry();
	
	ui->lblDevInfo01->setText("设备ID：" + ObfuscateString(QString::fromStdString(productId)));
	if (!deviceId.empty()) {
		std::string name = "huangjingpei";
		QString deviceNoBraces = removeBraces(QString::fromStdString(deviceId));
		QString obfuscateString = ObfuscateString(deviceNoBraces);
		ui->lblDevInfo02->setText("产品ID：" + obfuscateString);
	}


	QString plat = iniFile->value("DeviceData", "Alias.Plat", "DY").toString();
	int currentIndex = 0;
	if (plat == "DY") {
		plat = "抖音";
		currentIndex = 0;
	} else if (str1 == "KS") {
		plat = "快手";
		currentIndex = 1;
	} else if (str1 == "BILI") {
		plat = "哔哩哔哩";
		currentIndex = 2;
	} else if (str1 == "SPH") {
		plat = "视频号";
		currentIndex = 3;
	} else if (str1 == "PDD") {
		plat = "拼多多";
		currentIndex = 4;
	} else if (str1 == "TK") {
		plat = "TikTok";
		currentIndex = 5;
	} else if (str1 == "FB") {
		currentIndex = 6;
		plat = "Facobook";
	}
	ui->comboBox->setCurrentIndex(currentIndex);

	connect(ui->comboBox, &QComboBox::currentIndexChanged, this, [this](int index) {
		int currentIndex = index;
		QString str1 = "DY";
		if (currentIndex == 0) {
			str1 = "DY";
		} else if (currentIndex == 1) {
			str1 = "KS";
		} else if (currentIndex == 2) {
			str1 = "BILI";
		} else if (currentIndex == 3) {
			str1 = "SPH";
		} else if (currentIndex == 4) {
			str1 = "PDD";
		} else if (currentIndex == 5) {
			str1 = "TK";
		} else if (currentIndex == 6) {
			str1 = "FB";
		}
		std::unique_ptr<IniSettings> iniFile = std::make_unique<IniSettings>("gbs.ini");
		iniFile->setValue("DeviceData", "managed.Plat", str1);

	});
	
}
void GBSBizDeviceInfo::Update() {
	OBSBasic *main = reinterpret_cast<OBSBasic *>(App()->GetMainWindow());

		struct obs_video_info ovi = {};
	obs_get_video_info(&ovi);
		double curFPS = obs_get_active_fps();
		double obsFPS = (double)ovi.fps_num / (double)ovi.fps_den;
		QString str = QString::number(curFPS, 'f', 2);
		ui->lblMon02->setText(str);

	const char *path = main->GetCurrentOutputPath();
	double usage = os_cpu_usage_info_query(cpu_info);
	str = QString::number(usage, 'g', 2) + QStringLiteral("%");
	ui->lblMon01->setText(str);

#define MBYTE (1024ULL * 1024ULL)
#define GBYTE (1024ULL * 1024ULL * 1024ULL)
#define TBYTE (1024ULL * 1024ULL * 1024ULL * 1024ULL)
	num_bytes = os_get_free_disk_space(path);
	QString abrv = QStringLiteral(" MB");
	long double num;
	num = (long double)num_bytes / (1024.0l * 1024.0l);
	if (num_bytes > TBYTE) {
		num /= 1024.0l * 1024.0l;
		abrv = QStringLiteral(" TB");
	} else if (num_bytes > GBYTE) {
		num /= 1024.0l;
		abrv = QStringLiteral(" GB");
	}
	str = QString::number(num, 'f', 1) + abrv;
	ui->lblMon04->setText(str);

	num = (long double)os_get_proc_resident_size() / (1024.0l * 1024.0l);

	str = QString::number(num, 'f', 1) + QStringLiteral(" MB");
	ui->lblMon09->setText(str);

	num = (long double)obs_get_average_frame_time_ns() / 1000000.0l;

	str = QString::number(num, 'f', 1) + QStringLiteral(" ms");
	ui->lblMon05->setText(str);

		video_t *video = obs_get_video();
	uint32_t total_encoded = video_output_get_total_frames(video);
	uint32_t total_skipped = video_output_get_skipped_frames(video);

	if (total_encoded < first_encoded || total_skipped < first_skipped) {
		first_encoded = total_encoded;
		first_skipped = total_skipped;
	}
	total_encoded -= first_encoded;
	total_skipped -= first_skipped;

	num = total_encoded ? (long double)total_skipped / (long double)total_encoded : 0.0l;
	num *= 100.0l;

	str = QString("%1 / %2 (%3%)")
		      .arg(QString::number(total_skipped), QString::number(total_encoded),
			   QString::number(num, 'f', 1));
	ui->lblMon10->setText(str);


	
	uint32_t total_rendered = obs_get_total_frames();
	uint32_t total_lagged = obs_get_lagged_frames();

	if (total_rendered < first_rendered || total_lagged < first_lagged) {
		first_rendered = total_rendered;
		first_lagged = total_lagged;
	}
	total_rendered -= first_rendered;
	total_lagged -= first_lagged;

	num = total_rendered ? (long double)total_lagged / (long double)total_rendered : 0.0l;
	num *= 100.0l;

	str = MakeMissedFramesText(total_lagged, total_rendered, num);
	ui->lblMon08->setText(str);


}
void GBSBizDeviceInfo::RecordingTimeLeft() {

}


void GBSBizDeviceInfo::onSliderValueChanged(int value) {
    if (value == 0) {
        ui->horizontalSlider->setStyleSheet(
            "QSlider {"
            "    background: transparent;"
            "}"
            "QSlider::groove:horizontal {"
            "    border: none;"
            "    height: 20px;"
            "    margin: 0px;"
            "    border-radius: 13px;"
            "    background: 00C566;"
            "}"
            "QSlider::handle:horizontal {"
            "    background: white;"
            "    width: 26px;"
            "    height: 26px;"
            "    margin: -3px 0;"  // 调整滑块位置以适应槽
            "    border-radius: 13px;"
            "}"
            "QSlider::handle:horizontal:pressed {"
            "    width: 8px;" /* 保持滑块宽度一致 */
            "    height: 4px;" /* 保持滑块高度一致 */
            "    margin-left: -2px;" /* 保持外边距一致 */
            "    margin-right: -2px;" /* 保持外边距一致 */
            "}"
            "QSlider::add-page:horizontal {"
            "    background: #00C566;"
            "    border-radius: 2px;"
            "}"
            "QSlider::sub-page:horizontal {"
            "    background: #00C566;"
            "    border-radius: 2px;"
            "}"
            );

    } else {
        ui->horizontalSlider->setStyleSheet(
            "QSlider {"
            "    background: transparent;"
            "}"
            "QSlider::groove:horizontal {"
            "    border: none;"
            "    height: 20px;"
            "    margin: 0px;"
            "    border-radius: 13px;"
            "    background: qlineargradient(spread:pad, x1:0, y1:0.5, x2:1, y2:0.5, stop:0 #212121, stop:1 #000000);"
            "}"
            "QSlider::handle:horizontal {"
            "    background: white;"
            "    width: 26px;"
            "    height: 26px;"
            "    margin: -3px 0;"  // 调整滑块位置以适应槽
            "    border-radius: 13px;"
            "}"
            "QSlider::handle:horizontal:pressed {"
            "    width: 8px;" /* 保持滑块宽度一致 */
            "    height: 4px;" /* 保持滑块高度一致 */
            "    margin-left: -2px;" /* 保持外边距一致 */
            "    margin-right: -2px;" /* 保持外边距一致 */
            "}"
            "QSlider::add-page:horizontal {"
            "    background: qlineargradient(spread:pad, x1:0, y1:0.5, x2:1, y2:0.5, stop:0 #212121, stop:1 #000000);"
            "    border-radius: 2px;"
            "}"
            "QSlider::sub-page:horizontal {"
            "    background: qlineargradient(spread:pad, x1:0, y1:0.5, x2:1, y2:0.5, stop:0 #212121, stop:1 #000000);"
            "    border-radius: 2px;"
            "}"

            );
    }
}

GBSBizDeviceInfo::~GBSBizDeviceInfo()
{
	timer.stop();
	delete ui;
}
