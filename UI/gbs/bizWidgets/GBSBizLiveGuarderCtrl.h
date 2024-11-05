#ifndef GBSBIZLIVEGUARDERCTRL_H
#define GBSBIZLIVEGUARDERCTRL_H

#include <QWidget>

namespace Ui {
class GBSBizLiveGuarderCtrl;
}

class GBSBizLiveGuarderCtrl : public QWidget
{
    Q_OBJECT

public:
    explicit GBSBizLiveGuarderCtrl(QWidget *parent = nullptr);
~GBSBizLiveGuarderCtrl();

public slots:
void onTabChanged(int index);



private:
    Ui::GBSBizLiveGuarderCtrl *ui;

private:
	QWidget *validWidget = nullptr;
};

#endif // GBSBIZLIVEGUARDERCTRL_H
