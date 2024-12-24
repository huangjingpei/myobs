#include "gbsbizsettingav.h"
#include <sstream>

#include "ui_gbsbizsettingav.h"
#include "window-basic-main.hpp"
#include "window-basic-settings.hpp"
#include "qt-wrappers.hpp"


/* some nice default output resolution vals */
static const double vals[] = {1.0, 1.25, (1.0 / 0.75), 1.5, (1.0 / 0.6), 1.75, 2.0, 2.25, 2.5, 2.75, 3.0};

static const size_t numVals = sizeof(vals) / sizeof(double);

static std::string ResString(uint32_t cx, uint32_t cy)
{
	std::stringstream res;
	res << cx << "x" << cy;
	return res.str();
}
static std::tuple<int, int> aspect_ratio(int cx, int cy)
{
	int common = std::gcd(cx, cy);
	int newCX = cx / common;
	int newCY = cy / common;

	if (newCX == 8 && newCY == 5) {
		newCX = 16;
		newCY = 10;
	}

	return std::make_tuple(newCX, newCY);
}
static inline bool ResTooHigh(uint32_t cx, uint32_t cy)
{
	return cx > 16384 || cy > 16384;
}

static inline bool ResTooLow(uint32_t cx, uint32_t cy)
{
	return cx < 32 || cy < 32;
}

/* parses "[width]x[height]", string, i.e. 1024x768 */
static bool ConvertResText(const char *res, uint32_t &cx, uint32_t &cy)
{
	BaseLexer lex;
	base_token token;

	lexer_start(lex, res);

	/* parse width */
	if (!lexer_getbasetoken(lex, &token, IGNORE_WHITESPACE))
		return false;
	if (token.type != BASETOKEN_DIGIT)
		return false;

	cx = std::stoul(token.text.array);

	/* parse 'x' */
	if (!lexer_getbasetoken(lex, &token, IGNORE_WHITESPACE))
		return false;
	if (strref_cmpi(&token.text, "x") != 0)
		return false;

	/* parse height */
	if (!lexer_getbasetoken(lex, &token, IGNORE_WHITESPACE))
		return false;
	if (token.type != BASETOKEN_DIGIT)
		return false;

	cy = std::stoul(token.text.array);

	/* shouldn't be any more tokens after this */
	if (lexer_getbasetoken(lex, &token, IGNORE_WHITESPACE))
		return false;

	if (ResTooHigh(cx, cy) || ResTooLow(cx, cy)) {
		cx = cy = 0;
		return false;
	}

	return true;
}


void GBSBizSettingAV::LoadFPSCommon(OBSBasic *main)
{
	const char *val = config_get_string(main->Config(), "Video", "FPSCommon");

	int idx = ui->fpsCommon->findText(val);
	if (idx == -1)
		idx = 4;
	ui->fpsCommon->setCurrentIndex(idx);
}

void GBSBizSettingAV::LoadFPSInteger(OBSBasic *main)
{
	uint32_t val = config_get_uint(main->Config(), "Video", "FPSInt");
	ui->fpsInteger->setValue(val);
}

void GBSBizSettingAV::LoadFPSFraction(OBSBasic *main)
{
	uint32_t num = config_get_uint(main->Config(), "Video", "FPSNum");
	uint32_t den = config_get_uint(main->Config(), "Video", "FPSDen");

	ui->fpsNumerator->setValue(num);
	ui->fpsDenominator->setValue(den);
}

void GBSBizSettingAV::LoadFPSData()
{
	OBSBasic *main = OBSBasic::Get();
	LoadFPSCommon(main);
	LoadFPSInteger(main);
	LoadFPSFraction(main);

	uint32_t fpsType = config_get_uint(main->Config(), "Video", "FPSType");
	if (fpsType > 2)
		fpsType = 0;

	ui->cbxFpsType->setCurrentIndex(fpsType);
	ui->fpsTypes->setCurrentIndex(fpsType);
}


GBSBizSettingAV::GBSBizSettingAV(OBSBasicSettings *settings, QWidget *parent)
	: settings(settings),QWidget(parent),
	  ui(new Ui::GBSBizSettingAV)
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


	
	QComboBox *comboBox = settings->getBaseResolution();
	int count = comboBox->count();
	for (int i = 0; i < count; i++) {
		QString itemText = comboBox->itemText(i);
		ui->cbxBasicResolution->addItem(itemText);
	}
	QObject::connect(ui->cbxBasicResolution, &QComboBox::currentIndexChanged, this,
			 &GBSBizSettingAV::GeneralChanged);
	ui->cbxBasicResolution->setProperty("changed", QVariant(false));


	comboBox = settings->getOutputResolution();
	count = comboBox->count();
	for (int i = 0; i < count; i++) {
		QString itemText = comboBox->itemText(i);
		ui->cbxOutputResolution->addItem(itemText);
	}

	QObject::connect(ui->cbxOutputResolution, &QComboBox::currentIndexChanged, this,
			 &GBSBizSettingAV::GeneralChanged);
	ui->cbxOutputResolution->setProperty("changed", QVariant(false));

	comboBox = settings->getDownscaleFilter();
	count = comboBox->count();
	for (int i = 0; i < count; i++) {
		QString itemText = comboBox->itemText(i);
		ui->cbxScaleAlgo->addItem(itemText);
	}
	comboBox = settings->getFpsType();
	count = comboBox->count();
	for (int i = 0; i < count; i++) {
		QString itemText = comboBox->itemText(i);
		ui->cbxFpsType->addItem(itemText);
	}


	QLabel *label = settings->getBaseAspect();
	ui->label_4->setText(label->text());
	label = settings->getScaledAspect();
	ui->label_5->setText(label->text());

	ui->cbxBasicResolution->setStyleSheet(comboBoxStyleSheet);
	ui->cbxOutputResolution->setStyleSheet(comboBoxStyleSheet);
	ui->cbxScaleAlgo->setStyleSheet(comboBoxStyleSheet);
	ui->cbxFpsType->setStyleSheet(comboBoxStyleSheet);
	//QStackedWidget *stackedWidget = settings->getFpsTypes();
	//stackedWidget->setCurrentIndex(ui->cbxFpsType->currentIndex());

	connect(ui->cbxFpsType, &QComboBox::currentIndexChanged, this, [this](int index) {
		ui->fpsTypes->setCurrentIndex(index);
		QWidget *widget = ui->fpsTypes->currentWidget();
		for (int i = 0; i < widget->children().size(); ++i) {
			QSpinBox *spinBox = qobject_cast<QSpinBox *>(widget->children().at(i));
			if (spinBox) {
				spinBox->setStyleSheet(R"(
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
			}

QComboBox *comboBox = qobject_cast<QComboBox *>(widget->children().at(i));
if (comboBox) {
	comboBox->setStyleSheet(R"(
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
)");
			}
		}	
		});


	ui->cbxBasicResolution->setEditable(true);
	ui->cbxOutputResolution->setEditable(true);

	connect(ui->cbxBasicResolution, &QComboBox::currentTextChanged, this, [this](const QString &text) {
		
		on_cbxBasicResolution_editTextChanged(text);
		});

	connect(ui->cbxBasicResolution, &QComboBox::currentTextChanged, this,
		[this](const QString &text) { on_cbxOutputResolution_editTextChanged(text); });

	LoadResolutionLists();
	LoadFPSData();
	LoadDownscaleFilters();
	ui->fpsTypes->setStyleSheet("");
}

bool GBSBizSettingAV::ValidResolutions() {
	QString baseRes = ui->cbxBasicResolution->lineEdit()->text();
	uint32_t cx, cy;

	if (!ConvertResText(QT_TO_UTF8(baseRes), cx, cy)) {
		//ui->videoMsg->setText(QTStr(INVALID_RES_STR));
		return false;
	}

	bool lockedOutRes = !ui->cbxOutputResolution->isEditable();
	if (!lockedOutRes) {
		QString outRes = ui->cbxOutputResolution->lineEdit()->text();
		if (!ConvertResText(QT_TO_UTF8(outRes), cx, cy)) {
			//ui->videoMsg->setText(QTStr(INVALID_RES_STR));
			return false;
		}
	}

	//ui->videoMsg->setText("");
	return true;
}
void GBSBizSettingAV::LoadDownscaleFilters()
{
	OBSBasic* main = OBSBasic::Get();
	QString cbxScaleAlgo = ui->cbxScaleAlgo->currentData().toString();
	if (cbxScaleAlgo.isEmpty())
		cbxScaleAlgo = config_get_string(main->Config(), "Video", "ScaleType");

	ui->cbxScaleAlgo->clear();
	if (ui->cbxBasicResolution->currentText() == ui->cbxOutputResolution->currentText()) {
		ui->cbxScaleAlgo->setEnabled(false);
		ui->cbxScaleAlgo->addItem(QTStr("Basic.Settings.Video.DownscaleFilter.Unavailable"),
					     cbxScaleAlgo);
	} else {
		ui->cbxScaleAlgo->setEnabled(true);
		ui->cbxScaleAlgo->addItem(QTStr("Basic.Settings.Video.DownscaleFilter.Bilinear"),
					     QT_UTF8("bilinear"));
		ui->cbxScaleAlgo->addItem(QTStr("Basic.Settings.Video.DownscaleFilter.Area"), QT_UTF8("area"));
		ui->cbxScaleAlgo->addItem(QTStr("Basic.Settings.Video.DownscaleFilter.Bicubic"), QT_UTF8("bicubic"));
		ui->cbxScaleAlgo->addItem(QTStr("Basic.Settings.Video.DownscaleFilter.Lanczos"), QT_UTF8("lanczos"));

		if (cbxScaleAlgo == "bilinear")
			ui->cbxScaleAlgo->setCurrentIndex(0);
		else if (cbxScaleAlgo == "lanczos")
			ui->cbxScaleAlgo->setCurrentIndex(3);
		else if (cbxScaleAlgo == "area")
			ui->cbxScaleAlgo->setCurrentIndex(1);
		else
			ui->cbxScaleAlgo->setCurrentIndex(2);
	}
}

void GBSBizSettingAV::on_cbxBasicResolution_editTextChanged(const QString &text) {
	
	RecalcOutputResPixels(QT_TO_UTF8(text));
	LoadDownscaleFilters();
	
}
void GBSBizSettingAV::on_cbxOutputResolution_editTextChanged(const QString &text)
{
	if (!ValidResolutions()) {
		return ;
	}
	QString cbxBasicResolution = text;
	uint32_t cx, cy;

	ConvertResText(QT_TO_UTF8(cbxBasicResolution), cx, cy);

	std::tuple<int, int> aspect = aspect_ratio(cx, cy);

	ui->label_4->setText(
		QTStr("AspectRatio")
			.arg(QString::number(std::get<0>(aspect)), QString::number(std::get<1>(aspect))));

	ResetDownscales(cx, cy);
	
}


GBSBizSettingAV::~GBSBizSettingAV()
{
	delete ui;
}

void GBSBizSettingAV::LoadResolutionLists()
{
	OBSBasic *main = OBSBasic::Get();
	uint32_t cx = config_get_uint(main->Config(), "Video", "BaseCX");
	uint32_t cy = config_get_uint(main->Config(), "Video", "BaseCY");
	uint32_t out_cx = config_get_uint(main->Config(), "Video", "OutputCX");
	uint32_t out_cy = config_get_uint(main->Config(), "Video", "OutputCY");

	ui->cbxBasicResolution->clear();

	auto addRes = [this](int cx, int cy) {
		QString res = ResString(cx, cy).c_str();
		if (ui->cbxBasicResolution->findText(res) == -1)
			ui->cbxBasicResolution->addItem(res);
	};

	for (QScreen *screen : QGuiApplication::screens()) {
		QSize as = screen->size();
		uint32_t as_width = as.width();
		uint32_t as_height = as.height();

		// Calculate physical screen resolution based on the virtual screen resolution
		// They might differ if scaling is enabled, e.g. for HiDPI screens
		as_width = round(as_width * screen->devicePixelRatio());
		as_height = round(as_height * screen->devicePixelRatio());

		addRes(as_width, as_height);
	}

	addRes(1920, 1080);
	addRes(1280, 720);

	std::string outputResString = ResString(out_cx, out_cy);

	ui->cbxBasicResolution->lineEdit()->setText(ResString(cx, cy).c_str());

	RecalcOutputResPixels(outputResString.c_str());
	ResetDownscales(cx, cy, false);

	ui->cbxOutputResolution->lineEdit()->setText(outputResString.c_str());

	std::tuple<int, int> aspect = aspect_ratio(cx, cy);

	ui->label_4->setText(
		QTStr("AspectRatio").arg(QString::number(std::get<0>(aspect)), QString::number(std::get<1>(aspect))));
}

void GBSBizSettingAV::RecalcOutputResPixels(const char *resText)
{
	uint32_t newCX;
	uint32_t newCY;

	if (ConvertResText(resText, newCX, newCY) && newCX && newCY) {
		outputCX = newCX;
		outputCY = newCY;

		std::tuple<int, int> aspect = aspect_ratio(outputCX, outputCY);

		ui->label_5->setText(
			QTStr("AspectRatio")
				.arg(QString::number(std::get<0>(aspect)), QString::number(std::get<1>(aspect))));
	}
}


void GBSBizSettingAV::ResetDownscales(uint32_t cx, uint32_t cy, bool ignoreAllSignals)
{
	QString advRescale;
	QString advRecRescale;
	QString advFFRescale;
	QString oldOutputRes;
	std::string bestScale;
	int bestPixelDiff = 0x7FFFFFFF;
	uint32_t out_cx = outputCX;
	uint32_t out_cy = outputCY;



	bool lockedOutputRes = !ui->cbxOutputResolution->isEditable();

	if (!lockedOutputRes) {
		ui->cbxOutputResolution->blockSignals(true);
		ui->cbxOutputResolution->clear();
	}


	if (!out_cx || !out_cy) {
		out_cx = cx;
		out_cy = cy;
		oldOutputRes = ui->cbxBasicResolution->lineEdit()->text();
	} else {
		oldOutputRes = QString::number(out_cx) + "x" + QString::number(out_cy);
	}

	for (size_t idx = 0; idx < numVals; idx++) {
		uint32_t downscaleCX = uint32_t(double(cx) / vals[idx]);
		uint32_t downscaleCY = uint32_t(double(cy) / vals[idx]);
		uint32_t outDownscaleCX = uint32_t(double(out_cx) / vals[idx]);
		uint32_t outDownscaleCY = uint32_t(double(out_cy) / vals[idx]);

		downscaleCX &= 0xFFFFFFFC;
		downscaleCY &= 0xFFFFFFFE;
		outDownscaleCX &= 0xFFFFFFFE;
		outDownscaleCY &= 0xFFFFFFFE;

		std::string res = ResString(downscaleCX, downscaleCY);
		std::string outRes = ResString(outDownscaleCX, outDownscaleCY);
		if (!lockedOutputRes)
			ui->cbxOutputResolution->addItem(res.c_str());

		/* always try to find the closest output resolution to the
		 * previously set output resolution */
		int newPixelCount = int(downscaleCX * downscaleCY);
		int oldPixelCount = int(out_cx * out_cy);
		int diff = abs(newPixelCount - oldPixelCount);

		if (diff < bestPixelDiff) {
			bestScale = res;
			bestPixelDiff = diff;
		}
	}

	std::string res = ResString(cx, cy);

	if (!lockedOutputRes) {
		float baseAspect = float(cx) / float(cy);
		float outputAspect = float(out_cx) / float(out_cy);
		bool closeAspect = close_float(baseAspect, outputAspect, 0.01f);

		if (closeAspect) {
			ui->cbxOutputResolution->lineEdit()->setText(oldOutputRes);
			//on_cbxOutputResolution_editTextChanged(oldOutputRes);
		} else {
			ui->cbxOutputResolution->lineEdit()->setText(bestScale.c_str());
			//on_cbxOutputResolution_editTextChanged(bestScale.c_str());
		}

		ui->cbxOutputResolution->blockSignals(false);

		if (!closeAspect) {
			ui->cbxOutputResolution->setProperty("changed", QVariant(true));
			//videoChanged = true;
		}
	}

	if (advRescale.isEmpty())
		advRescale = res.c_str();
	if (advRecRescale.isEmpty())
		advRecRescale = res.c_str();
	if (advFFRescale.isEmpty())
		advFFRescale = res.c_str();


}

void GBSBizSettingAV::GeneralChanged()
{
	
	generalChanged = true;
	sender()->setProperty("changed", QVariant(true));
	
}

 bool GBSBizSettingAV::WidgetChanged(QWidget *widget)
{
	return widget->property("changed").toBool();
}

void GBSBizSettingAV::SaveVideoSettings()
{
	QString cbxBasicResolution = ui->cbxBasicResolution->currentText();
	QString cbxOutputResolution = ui->cbxOutputResolution->currentText();
	int fpsType = ui->fpsTypes->currentIndex();
	uint32_t cx = 0, cy = 0;
	OBSBasic *main = OBSBasic::Get();

	/* ------------------- */

	if (WidgetChanged(ui->cbxBasicResolution) && ConvertResText(QT_TO_UTF8(cbxBasicResolution), cx, cy)) {
		config_set_uint(main->Config(), "Video", "BaseCX", cx);
		config_set_uint(main->Config(), "Video", "BaseCY", cy);
	}

	if (WidgetChanged(ui->cbxOutputResolution) && ConvertResText(QT_TO_UTF8(cbxOutputResolution), cx, cy)) {
		config_set_uint(main->Config(), "Video", "OutputCX", cx);
		config_set_uint(main->Config(), "Video", "OutputCY", cy);
	}

	if (WidgetChanged(ui->fpsTypes))
		config_set_uint(main->Config(), "Video", "FPSType", fpsType);


	config_set_string(main->Config(), "Video", "ScaleType",
			  QT_TO_UTF8(ui->cbxScaleAlgo->itemData(ui->cbxScaleAlgo->currentIndex()).toString()));


	config_set_string(main->Config(), "Video", "ScaleType",
			QT_TO_UTF8(ui->cbxScaleAlgo->itemData(ui->cbxScaleAlgo->currentIndex()).toString()));
}

void GBSBizSettingAV::SaveAudioSettings() {

}


