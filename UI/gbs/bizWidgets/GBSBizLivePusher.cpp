#include "GBSBizLivePusher.h"
#include "ui_GBSBizLivePusher.h"
#include "GBSBizLivePusherAuth.h"
#include "GBSBizLivePusherCtrl.h"
#include <QList>
#include<QLineEdit>
GBSBizLivePusher::GBSBizLivePusher(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GBSBizLivePusher)
{
	ui->setupUi(this);
    auth = new GBSBizLivePusherAuth(this);

    int x = (width() - auth->width())/2;
    int y = (height() - auth->height())/2;
    auth->setGeometry(x, y, auth->width(), auth->height());

    //ui->horizontalLayout->addWidget(auth, 20);


}

void GBSBizLivePusher::enterGuarderCtrl() {

	delete reinterpret_cast<GBSBizLivePusherAuth *>(sender());
    //reinterpret_cast<GBSBizLivePusherAuth *>(sender())->deleteLater();
    GBSBizLivePusherCtrl *ctrl = new GBSBizLivePusherCtrl(this);
    ui->horizontalLayout->addWidget(ctrl);
}

GBSBizLivePusher::~GBSBizLivePusher()
{
	delete ui;
}
