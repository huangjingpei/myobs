#include "gbsbizsettingav.h"
#include "ui_gbsbizsettingav.h"
#include "window-basic-main.hpp"
#include "window-basic-settings.hpp"
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

	comboBox = settings->getOutputResolution();
	count = comboBox->count();
	for (int i = 0; i < count; i++) {
		QString itemText = comboBox->itemText(i);
		ui->cbxOutputResolution->addItem(itemText);
	}
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

	connect(ui->cbxBasicResolution, &QComboBox::currentTextChanged, this,
		[settings, this](const QString &text) {
			settings->on_baseResolution_editTextChanged(text);
		});


}



GBSBizSettingAV::~GBSBizSettingAV()
{
	delete ui;
}


