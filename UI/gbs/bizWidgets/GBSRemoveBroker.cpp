#include "GBSRemoveBroker.h"
#include "ui_GBSRemoveBroker.h"

GBSRemoveBroker::GBSRemoveBroker(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GBSRemoveBroker)
{
    ui->setupUi(this);

	ui->widget->setStyleSheet("QWidget {"
				"background-color:#0xFFFFFF;"
				"border:none;"
				"}");
	ui->widget_2->setStyleSheet("QWidget { "
					"background: qlineargradient("
					"spread:pad, "
					"x1:0, y1:0, x2:1, y2:1, "
					"stop:0.1543 rgba(237, 255, 246, 255), "
					"stop:0.822 rgba(255, 255, 255, 255)"
					"); "
					"border: none; "
				"}"
				    "#myButton {"
				    "background-color:#00C566;"
				    "border: 1px solid white;"
				    "border-radius:10px;"
				    "font-size:20px;"
				    "}"
				    "#myButton:pressed {"
				    "   padding-left: 3px;" // 向左移动 3px
				    "   padding-top: 3px;"  // 向上移动 3px
				    "   background-repeat: no-repeat;"
				    "   background-position: center;"
				    "}"
	);
	ui->btnOK->setObjectName("myButton");
	ui->btnCancel->setObjectName("myButton");
	ui->btnClose->setStyleSheet("QPushButton {"
				    "   background-image: url(:gbs/images/gbs/biz/gbs-close-window.png);"
				    "  border-radius: 5px;" // 圆角
				    "}"
				    "QPushButton:hover {"
				    "  border: none;"
				    "   background-color: #F9F9F9;"
				    "}"
				    "QPushButton:pressed {"
				    "   background-color: #D1D8DD;" // 按下时背景颜色
				    "   padding-left: 3px;"         // 向左移动 3px
				    "   padding-top: 3px;"          // 向上移动 3px
				    "   background-repeat: no-repeat;"
				    "   background-position: center;"
				    "}");
}

GBSRemoveBroker::~GBSRemoveBroker()
{
    delete ui;
}
