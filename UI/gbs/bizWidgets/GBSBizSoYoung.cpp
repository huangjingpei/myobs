#include "GBSBizSoYoung.h"
#include "ui_GBSBizSoYoung.h"

//So Young, So Night ! mark it at 2024-10-10T10:29.
GBSBizSoYoung::GBSBizSoYoung(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GBSBizSoYoung)
{
	ui->setupUi(this);
	QString soNight = R"(
	    <p style="font-size: 32px; text-align: center;">
		<span style="color: #9CA4AB;">感谢你的关注，我们已经在努力开发中，敬请期待未来的版本！</span>
	    </p>
	)";
	ui->lblSoNight->setText(soNight);
}

GBSBizSoYoung::~GBSBizSoYoung()
{
	delete ui;
}
