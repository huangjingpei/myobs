#ifndef GBSBIZLIVEPUSHER_H
#define GBSBIZLIVEPUSHER_H

#include <QWidget>
#include <QKeyEvent>
#include <QList>
#include <QLineEdit>
#include "GBSBizLivePusherAuth.h"

namespace Ui {
class GBSBizLivePusher;
}

class GBSBizLivePusher : public QWidget {
	Q_OBJECT

public:
	explicit GBSBizLivePusher(QWidget *parent = nullptr);
	~GBSBizLivePusher();


public slots:
    void enterGuarderCtrl();

private:
	Ui::GBSBizLivePusher *ui;


    GBSBizLivePusherAuth *auth;



};

#endif // GBSBIZLIVEPUSHER_H
