#ifndef GBSBIZLIVEGUARDER_H
#define GBSBIZLIVEGUARDER_H

#include <QWidget>
#include <QKeyEvent>
#include <QList>
#include <QLineEdit>
#include "GBSBizLiveGuarderAuth.h"

namespace Ui {
class GBSBizLiveGuarder;
}

class GBSBizLiveGuarder : public QWidget {
	Q_OBJECT

public:
	explicit GBSBizLiveGuarder(QWidget *parent = nullptr);
	~GBSBizLiveGuarder();


public slots:
    void enterGuarderCtrl();

private:
	Ui::GBSBizLiveGuarder *ui;


    GBSBizLiveGuarderAuth *auth;



};

#endif // GBSBIZLIVEGUARDER_H
