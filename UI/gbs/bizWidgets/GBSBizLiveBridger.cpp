#include "GBSBizLiveBridger.h"
#include "ui_GBSBizLiveBridger.h"


#include <QVBoxLayout>
#include <QFrame>
#include "../common/VertNaviButton.h"
#include "../common/VertNaviButtonExt.h"
#include "../common/VolumeHBarWidget.h"

GBSBizLiveBridger::GBSBizLiveBridger(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GBSBizLiveBridger)
{
	ui->setupUi(this);


	QVBoxLayout *leftLocalSourceLayout = new QVBoxLayout();
	VertNaviButton* tx = new VertNaviButton("图像", ":gbs/images/gbs/biz/gs-local-source-tx.png", this);
	VertNaviButton* mty = new VertNaviButton("媒体源", ":gbs/images/gbs/biz/gs-local-source-mty.png", this);
	VertNaviButton* wb = new VertNaviButton("文本", ":gbs/images/gbs/biz/gs-local-source-wb.png", this);
	VertNaviButton* xsqcj = new VertNaviButton("显示器采集", ":gbs/images/gbs/biz/gs-local-source-xsqcj.png", this);
	VertNaviButton* llq = new VertNaviButton("浏览器", ":gbs/images/gbs/biz/gs-local-source-llq.png", this);
	VertNaviButton* kkcj = new VertNaviButton("窗口采集", ":gbs/images/gbs/biz/gs-local-source-ckcj.png", this);
	VertNaviButton* sy = new VertNaviButton("色源", ":gbs/images/gbs/biz/gs-local-source-sy.png", this);
	VertNaviButton* sxt = new VertNaviButton("摄像头", ":gbs/images/gbs/biz/gs-local-source-sxt.png", this);
	VertNaviButton* ypsc = new VertNaviButton("音频输出", ":gbs/images/gbs/biz/gs-local-source-ypsc.png", this);
	VertNaviButton* ypsr = new VertNaviButton("音频输入", ":gbs/images/gbs/biz/gs-local-source-ypsr.png", this);
	VertNaviButton* yxcj = new VertNaviButton("游戏采集", ":gbs/images/gbs/biz/gs-local-source-yxcj.png", this);
	leftLocalSourceLayout->addWidget(tx);
	leftLocalSourceLayout->addWidget(mty);
	leftLocalSourceLayout->addWidget(wb);
	leftLocalSourceLayout->addWidget(xsqcj);
	leftLocalSourceLayout->addWidget(llq);
	leftLocalSourceLayout->addWidget(kkcj);
	leftLocalSourceLayout->addWidget(sy);
	leftLocalSourceLayout->addWidget(sxt);
	leftLocalSourceLayout->addWidget(ypsc);
	leftLocalSourceLayout->addWidget(ypsr);
	leftLocalSourceLayout->addWidget(yxcj);


	tx->setFixedSize(180, 30);
	mty->setFixedSize(180, 30);
	wb->setFixedSize(180, 30);
	xsqcj->setFixedSize(180, 30);
	llq->setFixedSize(180, 30);
	kkcj->setFixedSize(180, 30);
	sy->setFixedSize(180, 30);
	sxt->setFixedSize(180, 30);
	ypsc->setFixedSize(180, 30);
	ypsr->setFixedSize(180, 30);
	yxcj->setFixedSize(180, 30);



	QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	leftLocalSourceLayout->addSpacerItem(verticalSpacer);

	ui->horiLocalSourceLayout->addLayout(leftLocalSourceLayout);




	// 添加竖线
	QFrame *line = new QFrame(this);
	line->setFrameShape(QFrame::VLine);
	line->setLineWidth(2);
	line->setPalette(QPalette(Qt::lightGray)); // 设置线的颜色
	ui->horiLocalSourceLayout->addWidget(line);

	QVBoxLayout *rightLocalSourceLayout = new QVBoxLayout();
	VertNaviButtonExt* ycdb = new VertNaviButtonExt("远程代播", ":gbs/images/gbs/biz/gs-local-source-tx.png", this);
	VertNaviButtonExt* tx2 = new VertNaviButtonExt("图像", ":gbs/images/gbs/biz/gs-local-source-tx.png", this);
	VertNaviButtonExt* mty2 = new VertNaviButtonExt("媒体源", ":gbs/images/gbs/biz/gs-local-source-mty.png", this);
	VertNaviButtonExt* wb2 = new VertNaviButtonExt("文本", ":gbs/images/gbs/biz/gs-local-source-wb.png", this);
	VertNaviButtonExt* xsqcj2 = new VertNaviButtonExt("显示器采集", ":gbs/images/gbs/biz/gs-local-source-xsqcj.png", this);
	VertNaviButtonExt* llq2 = new VertNaviButtonExt("浏览器", ":gbs/images/gbs/biz/gs-local-source-llq.png", this);
	VertNaviButtonExt* kkcj2 = new VertNaviButtonExt("窗口采集", ":gbs/images/gbs/biz/gs-local-source-ckcj.png", this);
	VertNaviButtonExt* sy2 = new VertNaviButtonExt("色源", ":gbs/images/gbs/biz/gs-local-source-sy.png", this);
	VertNaviButtonExt* sxt2 = new VertNaviButtonExt("摄像头", ":gbs/images/gbs/biz/gs-local-source-sxt.png", this);
	VertNaviButtonExt* ypsc2 = new VertNaviButtonExt("音频输出", ":gbs/images/gbs/biz/gs-local-source-ypsc.png", this);
	VertNaviButtonExt* ypsr2 = new VertNaviButtonExt("音频输入", ":gbs/images/gbs/biz/gs-local-source-ypsr.png", this);
	VertNaviButtonExt* yxcj2 = new VertNaviButtonExt("游戏采集", ":gbs/images/gbs/biz/gs-local-source-yxcj.png", this);

	rightLocalSourceLayout->addWidget(ycdb);
	rightLocalSourceLayout->addWidget(tx2);
	rightLocalSourceLayout->addWidget(mty2);
	rightLocalSourceLayout->addWidget(wb2);
	rightLocalSourceLayout->addWidget(xsqcj2);
	rightLocalSourceLayout->addWidget(llq2);
	rightLocalSourceLayout->addWidget(kkcj2);
	rightLocalSourceLayout->addWidget(sy2);
	rightLocalSourceLayout->addWidget(sxt2);
	rightLocalSourceLayout->addWidget(ypsc2);
	rightLocalSourceLayout->addWidget(ypsr2);
	rightLocalSourceLayout->addWidget(yxcj2);

	ycdb->setFixedSize(224, 30);
	tx2->setFixedSize(224, 30);
	mty2->setFixedSize(224, 30);
	wb2->setFixedSize(224, 30);
	xsqcj2->setFixedSize(224, 30);
	llq2->setFixedSize(224, 30);
	kkcj2->setFixedSize(224, 30);
	sy2->setFixedSize(224, 30);
	sxt2->setFixedSize(224, 30);
	ypsc2->setFixedSize(224, 30);
	ypsr2->setFixedSize(224, 30);
	yxcj2->setFixedSize(224, 30);


	QSpacerItem* verticalSpacer2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	leftLocalSourceLayout->addSpacerItem(verticalSpacer2);
	ui->horiLocalSourceLayout->addLayout(rightLocalSourceLayout);

	ui->frmLocalSource->setLayout(ui->horiLocalSourceLayout);


	VolumeHBarWidget* microphone = new VolumeHBarWidget("Microphone", ":gbs/images/gbs/biz/gbs-microphone-open.png", this);
	VolumeHBarWidget* speaker = new VolumeHBarWidget("speaker", ":gbs/images/gbs/biz/gbs-speaker-open.png", this);
	ui->vertAudioLayout->setSpacing(5);
	ui->vertAudioLayout->addWidget(microphone);
	ui->vertAudioLayout->addWidget(speaker);



	// QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	// ui->vertAudioLayout->addSpacerItem(verticalSpacer);


}

GBSBizLiveBridger::~GBSBizLiveBridger()
{
	delete ui;
}
