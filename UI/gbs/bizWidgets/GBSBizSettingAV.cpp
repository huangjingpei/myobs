#include "gbsbizsettingav.h"
#include "ui_gbsbizsettingav.h"
#include "window-basic-main.hpp"
#include "window-basic-settings.hpp"
GBSBizSettingAV::GBSBizSettingAV(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GBSBizSettingAV)
{
	ui->setupUi(this);



	ui->tabWidget->setStyleSheet(
		"QTabWidget::pane {"
		"    border: transparent;"  // 移除tab pane的边框
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





	//connect(ui->tabWidget, &QTabWidget::currentChanged, this,
	//	[this](int index) {
	//		qDebug() << "Current tab index is:" << index;
	//	if (index == 0) {
	//			OBSBasic *main = OBSBasic::Get();
	//			if (main != nullptr) {
	//				OBSBasicSettings settings(main);
	//				settings.setWindowFlags(Qt::Widget);
	//				ui->horizontalLayout->addWidget(&settings);
	//				//settings.show();
	//			}
	//	} 
	//	});

}



GBSBizSettingAV::~GBSBizSettingAV()
{
	delete ui;
}
