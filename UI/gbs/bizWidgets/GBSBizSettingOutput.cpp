#include "GBSBizSettingOutput.h"
#include "ui_GBSBizSettingOutput.h"

GBSBizSettingOutput::GBSBizSettingOutput(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GBSBizSettingOutput)
{
	ui->setupUi(this);
	QString comboBoxStyleSheet = R"(
	    QComboBox {
		border: 1px solid gray;
		border-radius: 3px;
		padding: 5px;
	    }

	    /* 设置 QComboBox 的箭头颜色和大小 */
	    QComboBox::down-arrow {
		image: none;  /* 取消默认图标 */
		border: none;
		width: 20px;   /* 设置箭头宽度 */
		height: 20px;  /* 设置箭头高度 */
		image: url(:gbs/images/gbs/biz/gbs-arrow-down-green.png);  /* 替换为你想要的箭头图片 */

	    }

	    QComboBox::down-arrow:enabled {
		border: none;
		image: url(:gbs/images/gbs/biz/gbs-arrow-down-green.png);  /* 替换为你想要的箭头图片 */
	    }
	)";

	ui->cbxAudioEncoder->setStyleSheet(comboBoxStyleSheet);
	ui->cbxVideoBitrate->setStyleSheet(comboBoxStyleSheet);
	ui->cbxVideoEncoder->setStyleSheet(comboBoxStyleSheet);
	ui->cbxVideoEncoderPreSetting->setStyleSheet(comboBoxStyleSheet);
	ui->cbxRecordQuality->setStyleSheet(comboBoxStyleSheet);
	ui->cbxAudioBitrate->setStyleSheet(comboBoxStyleSheet);
	ui->cbxRecordFomat->setStyleSheet(comboBoxStyleSheet);



	ui->tabWidget->setStyleSheet(
		"QTabWidget::pane {"
		"    border: none;"  // 移除tab pane的边框
		"}"
		"QTabBar::tab {"
		"    background: none;"  // tab的背景颜色
		"    padding: 10px;"  // tab内容的填充
		"    margin-right: 10px;"  // 调整tab之间的水平间距
		"    border: none;"  // 移除tab的边框
		"    color: #01C667;"
		"    font-size:16px;"
		"}"
		"QTabBar::tab:first {"
		"    margin-left: 100px;"  // 调整第一个tab项的左外边距
		"}"
		"QTabBar::tab:selected {"
		"    background: #C0C0C0;"  // 选中tab的背景颜色
		"    border: none;"  // 选中时也不显示边框
		"    border-radius: 16px"
		"}"
		"QTabBar {"
		"    qproperty-alignment: AlignCenter;"  // 设置tab项居中对齐
		"}"
		);


}


GBSBizSettingOutput::~GBSBizSettingOutput()
{
	delete ui;
}

