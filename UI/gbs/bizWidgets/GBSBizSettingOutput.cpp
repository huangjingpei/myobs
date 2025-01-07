#include "GBSBizSettingOutput.h"
#include "ui_GBSBizSettingOutput.h"
#include "window-basic-main.hpp"
#include "window-basic-settings.hpp"


GBSBizSettingOutput::GBSBizSettingOutput(QSharedPointer<OBSBasicSettings> settings, QWidget *parent)
	:settings(settings), QWidget(parent),
	  ui(new Ui::GBSBizSettingOutput)
{
	ui->setupUi(this);
	QString comboBoxStyleSheet = R"(
    /* QComboBox整体样式 */
    QComboBox {
        border: 1px solid #cccccc;       /* 边框颜色 */
        border-radius: 5px;             /* 边框圆角 */
        padding: 5px;                   /* 内边距 */
        background-color: #ffffff;      /* 背景颜色 */
    }

    /* 替换下拉箭头的样式 */
    QComboBox::down-arrow {
        width: 20px;                    /* 箭头宽度 */
        height: 20px;                   /* 箭头高度 */
        image: url(:gbs/images/gbs/biz/gbs-combox-down.png); /* 替换为图片路径 */
    }

    /* 调整下拉箭头的位置 */
    QComboBox::down-arrow:hover {
        image: url(:gbs/images/gbs/biz/gbs-combox-down.png); /* 鼠标悬浮时图片 */
    }

    /* 下拉按钮的区域（对齐和大小调整） */
    QComboBox::drop-down {
        border: none;                   /* 移除边框 */
        width: 30px;                    /* 下拉按钮区域宽度 */
        subcontrol-origin: padding;     /* 基于内边距布局 */
        subcontrol-position: top right; /* 对齐到右上角 */
    }

    /* 下拉列表项的样式 */
    QComboBox QAbstractItemView {
        border: 1px solid #cccccc;      /* 下拉列表的边框 */
        selection-background-color: #1B2846; /* 选中项背景色 */
        selection-color: white;         /* 选中项文本颜色 */
    }
)";

	ui->cbxAudioEncoder->setStyleSheet(comboBoxStyleSheet);
	ui->cbxVideoEncoder->setStyleSheet(comboBoxStyleSheet);
	ui->cbxVideoEncoderPreSetting->setStyleSheet(comboBoxStyleSheet);
	ui->cbxRecordQuality->setStyleSheet(comboBoxStyleSheet);
	ui->cbxAudioBitrate->setStyleSheet(comboBoxStyleSheet);
	ui->cbxRecordFomat->setStyleSheet(comboBoxStyleSheet);
	ui->spinBoxVideoBitrate->setStyleSheet(R"(
	    QSpinBox {
		border: 1px solid gray;
		border-radius: 3px;
		padding: 5px;
	    }
	    QSpinBox::up-button {
		image: url(:gbs/images/gbs/biz/gbs-spinbox-up.png);
		width: 16px;
		height: 20px;
	    }
	    QSpinBox::down-button {
		image: url(:gbs/images/gbs/biz/gbs-spinbox-down.png);
		width: 16px;
		height: 20px;
	    }

	    QSpinBox::up-button:pressed {
		margin-right: -1px; /* 向右移动 3 个像素 */
	    }
	    QSpinBox::down-button:pressed {
		margin-right: -1px; /* 向右移动 3 个像素 */
	    }
	)");

	


	#define PRESET_STR(val) QString(Str("Basic.Settings.Output.EncoderPreset." val)).arg(val)
	ui->cbxVideoEncoderPreSetting->addItem(PRESET_STR("ultrafast"), "ultrafast");
	ui->cbxVideoEncoderPreSetting->addItem("superfast", "superfast");
	ui->cbxVideoEncoderPreSetting->addItem(PRESET_STR("veryfast"), "veryfast");
	ui->cbxVideoEncoderPreSetting->addItem("faster", "faster");
	ui->cbxVideoEncoderPreSetting->addItem(PRESET_STR("fast"), "fast");
	#undef PRESET_STR


	OBSBasic *main = OBSBasic::Get();

	const char *path = config_get_string(main->Config(), "AdvOut", "RecFilePath");
	int videoBitrate = config_get_uint(main->Config(), "SimpleOutput", "VBitrate");
	int audioBitrate = config_get_uint(main->Config(), "SimpleOutput", "ABitrate");
	const char *recQual = config_get_string(main->Config(), "SimpleOutput", "RecQuality");
	const char *format = config_get_string(main->Config(), "SimpleOutput", "RecFormat2");
	int rbTime = config_get_int(main->Config(), "SimpleOutput", "RecRBTime");
	const char *streamEnc = config_get_string(main->Config(), "SimpleOutput", "StreamEncoder");
	const char *preset = config_get_string(main->Config(), "SimpleOutput", "Preset");
	const char *streamAudioEnc = config_get_string(main->Config(), "SimpleOutput", "StreamAudioEncoder");

	


	QComboBox *comboBox = settings->getSimpleOutStrEncoder();
	int count = comboBox->count();
	for (int i = 0; i < count; i++) {
		QString itemText = comboBox->itemText(i);
		ui->cbxVideoEncoder->addItem(itemText);
	}


	comboBox = settings->getSimpleOutputABitrate();
	count = comboBox->count();
	for (int i = 0; i < count; i++) {
		QString itemText = comboBox->itemText(i);
		ui->cbxAudioBitrate->addItem(itemText);
	}
	
	
	comboBox = settings->getSimpleOutPreset();
	count = comboBox->count();
	for (int i = 0; i < count; i++) {
		QString itemText = comboBox->itemText(i);
		ui->cbxVideoEncoderPreSetting->addItem(itemText);
	}

	comboBox = settings->getSimpleOutStrAEncoder();
	count = comboBox->count();
	for (int i = 0; i < count; i++) {
		QString itemText = comboBox->itemText(i);
		ui->cbxAudioEncoder->addItem(itemText);
	}

	comboBox = settings->getSimpleOutRecQuality();
	count = comboBox->count();
	for (int i = 0; i < count; i++) {
		QString itemText = comboBox->itemText(i);
		ui->cbxRecordQuality->addItem(itemText);
	}

	comboBox = settings->getSimpleOutRecFormat();
	count = comboBox->count();
	for (int i = 0; i < count; i++) {
		QString itemText = comboBox->itemText(i);
		ui->cbxRecordFomat->addItem(itemText);
	}

	ui->spinBoxVideoBitrate->setSingleStep(50);
	ui->spinBoxVideoBitrate->setSuffix(" Kbps");
	ui->spinBoxVideoBitrate->setValue(videoBitrate);
	ui->cbxAudioBitrate->setCurrentText(QString("%1").arg(audioBitrate));
	ui->cbxAudioBitrate->setCurrentText(QString("%1").arg(streamEnc));
	ui->cbxVideoEncoderPreSetting->setCurrentText(QString("%1").arg(preset));
	ui->cbxAudioEncoder->setCurrentText(QString("%1").arg(streamAudioEnc));
	ui->cbxRecordQuality->setCurrentText(QString("%1").arg(recQual));
	ui->cbxRecordFomat->setCurrentText(QString("%1").arg(format));
	ui->leRecordCache->setText(QString("%1").arg(rbTime));

	ui->leRecordPath->setText(QString("%1").arg(path));


	connect(ui->btnOpen, &QPushButton::clicked, this, [main, this]() {
		if (main != nullptr) {
			OBSBasicSettings settings(main);
			//QString dir = settings.setOutRecPathBrowse();
			//ui->leRecordPath->setText(dir);
		}
	});




}


GBSBizSettingOutput::~GBSBizSettingOutput()
{
	delete ui;
}

void GBSBizSettingOutput::SaveOutputSettings() {

}

