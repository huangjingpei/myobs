#include "GBSBizLiveGuarder.h"
#include "ui_GBSBizLiveGuarder.h"
#include "GBSBizLiveGuarderAuth.h"
#include "GBSBizLiveGuarderCtrl.h"
#include <QList>
#include<QLineEdit>
#define AUTH_CODE_NUM 5
GBSBizLiveGuarder::GBSBizLiveGuarder(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::GBSBizLiveGuarder)
{
	ui->setupUi(this);
    auth = new GBSBizLiveGuarderAuth(this);

    int x = (width() - auth->width())/2;
    int y = (height() - auth->height())/2;
    auth->setGeometry(x, y, auth->width(), auth->height());

    //ui->horizontalLayout->addWidget(auth, 20);


}

void GBSBizLiveGuarder::enterGuarderCtrl() {

    reinterpret_cast<GBSBizLiveGuarderAuth *>(sender())->deleteLater();
    GBSBizLiveGuarderCtrl *ctrl = new GBSBizLiveGuarderCtrl(this);
    ui->horizontalLayout->addWidget(ctrl);
}

GBSBizLiveGuarder::~GBSBizLiveGuarder()
{
	delete ui;
}
